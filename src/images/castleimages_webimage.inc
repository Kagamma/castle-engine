{
  Copyright 2022-2022 Trung Le (kagamma).

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Image oparations using WebImage

  WebImage is a wrapper of https://www.npmjs.com/package/image-decode, which in turn a
  wrapper or various JS image decoders
}

{$if defined(FPC) and defined(FPC_WASI)} // WebImage is only available in FPC web target

function CreateWebImage(const Width, Height: Integer): TInternalCastleWebImage;
begin
  Result := TInternalCastleWebImage.Create(Width, Height);
end;

{ TInternalCastleWebImage -> TEncodedImage --------------------------------------------- }

procedure TEncodedImage.FromWebImage(const WebImage: TInternalCastleWebImage);
begin
  NotImplemented('FromWebImage');
end;

procedure TRGBImage.FromWebImage(const WebImage: TInternalCastleWebImage);
var
  X, Y: Integer;
  ResultPixels: PVector3Byte;
  Color: TWebColorRGBAValue;
begin
  SetSize(WebImage.Width, WebImage.Height);
  ResultPixels := Pixels;
  for Y := WebImage.Height - 1 downto 0 do
    for X := 0 to WebImage.Width - 1 do
    begin
      Color := WebImage.Colors[X, Y];
      ResultPixels^.Data[0] := Color.R;
      ResultPixels^.Data[1] := Color.G;
      ResultPixels^.Data[2] := Color.B;
      Inc(ResultPixels);
    end;
end;

procedure TRGBAlphaImage.FromWebImage(const WebImage: TInternalCastleWebImage);
var
  Y: Integer;
  RowSize: Integer;
  SourceRow: PByte;
  DestRow: PByte;
begin
  SetSize(WebImage.Width, WebImage.Height);

  RowSize := WebImage.Width * 4;
  { prepare pointers }
  SourceRow := PByte(WebImage.FData) + RowSize * WebImage.Height;
  DestRow := FRawPixels;

  { copy lines upside down }
  for Y := WebImage.Height - 1 downto 0 do
  begin
    Dec(SourceRow, RowSize);
    Move(SourceRow^, DestRow^, RowSize);
    Inc(DestRow, RowSize);
  end;
end;

procedure TGrayscaleImage.FromWebImage(const WebImage: TInternalCastleWebImage);
var
  X, Y: Integer;
  ResultPixels: PByte;
  Color: TWebColorRGBAValue;
begin
  SetSize(WebImage.Width, WebImage.Height);

  ResultPixels := Pixels;
  for Y := WebImage.Height - 1 downto 0 do
    for X := 0 to WebImage.Width - 1 do
    begin
      Color := WebImage.Colors[X, Y];
      ResultPixels^ := GrayscaleValue(Vector3Byte(
        Color.R,
        Color.G,
        Color.B));
      Inc(ResultPixels);
    end;
end;

procedure TGrayscaleAlphaImage.FromWebImage(const WebImage: TInternalCastleWebImage);
var
  X, Y: Integer;
  ResultPixels: PVector2Byte;
  Color: TWebColorRGBAValue;
begin
  SetSize(WebImage.Width, WebImage.Height);

  ResultPixels := Pixels;
  for Y := WebImage.Height - 1 downto 0 do
    for X := 0 to WebImage.Width - 1 do
    begin
      Color := WebImage.Colors[X, Y];
      ResultPixels^.Data[0] := GrayscaleValue(Vector3Byte(
        Color.R,
        Color.G,
        Color.B));
      ResultPixels^.Data[1] := Color.A;
      Inc(ResultPixels);
    end;
end;

procedure TRGBFloatImage.FromWebImage(const WebImage: TInternalCastleWebImage);
var
  X, Y: Integer;
  ResultPixels: PVector3;
  Color: TWebColorRGBAValue;
begin
  SetSize(WebImage.Width, WebImage.Height);

  ResultPixels := Pixels;
  for Y := WebImage.Height - 1 downto 0 do
    for X := 0 to WebImage.Width - 1 do
    begin
      Color := WebImage.Colors[X, Y];
      ResultPixels^.Data[0] := Color.R / 255;
      ResultPixels^.Data[1] := Color.G / 255;
      ResultPixels^.Data[2] := Color.B / 255;
      Inc(ResultPixels);
    end;
end;


{ Convert TInternalCastleWebImage to the best TCastleImage class. }
function CreateFromWebImage(const WebImage: TInternalCastleWebImage;
  const AllowedImageClasses: array of TEncodedImageClass): TEncodedImage;

  { WebImage doesn't provide us any information does the image contain useful
    alpha channel, and is it grayscale.
    So just iterate over pixels, and check it ourselves. }
  procedure Detect(out Alpha, Grayscale: boolean);
  var
    X, Y: Integer;
    Col: TWebColorRGBAValue;
  begin
    Alpha := false;
    Grayscale := true;
    for Y := WebImage.Height - 1 downto 0 do
      for X := 0 to WebImage.Width - 1 do
      begin
        Col := WebImage.Colors[X, Y];
        if Col.A <> $FF then
        begin
          Alpha := true;
          if not Grayscale then Exit; // early exit for RGBA images
        end;
        if (Col.R <> Col.G) or
           (Col.R <> Col.B) then
        begin
          Grayscale := false;
          if Alpha then Exit; // early exit for RGBA images
        end;
      end;
  end;

var
  Alpha, Grayscale: boolean;
  ImageClass: TEncodedImageClass;
begin
  if High(AllowedImageClasses) = 0 then
  begin
    { if AllowedImageClasses forces a specific class, then just create this image class }
    ImageClass := AllowedImageClasses[0];
  end else
  begin
    { detect Alpha and Grayscale looking at image contents, to determine ImageClass }
    Detect(Alpha, Grayscale);
    if not Grayscale then
    begin
      if Alpha then
        ImageClass := TRGBAlphaImage
      else
        ImageClass := TRGBImage;
    end else
    begin
      if Alpha then
        ImageClass := TGrayscaleAlphaImage
      else
        ImageClass := TGrayscaleImage;
    end;

    { if the optimal ImageClass (for these image contents) is not allowed,
      try to find another class that works, and looses least information. }
    if not ClassAllowed(ImageClass, AllowedImageClasses) then
    begin
      if ClassAllowed(TRGBAlphaImage, AllowedImageClasses) then
        ImageClass := TRGBAlphaImage
      else
      if ClassAllowed(TRGBImage, AllowedImageClasses) then
        ImageClass := TRGBImage
      else
      if ClassAllowed(TGrayscaleImage, AllowedImageClasses) then
        ImageClass := TGrayscaleImage
      else
      if ClassAllowed(TGrayscaleAlphaImage, AllowedImageClasses) then
        ImageClass := TGrayscaleAlphaImage
      else
        raise Exception.Create('Cannot load WebImage to any of formats allowed by LoadImage AllowedImageClasses');
    end;
  end;

  Result := ImageClass.Create;
  try
    Result.FromWebImage(WebImage);
  except FreeAndNil(Result); raise end;
end;

function Load_WebImage(const Stream: TStream;
  const AllowedImageClasses: array of TEncodedImageClass;
  const ImageFormat: TImageFormat): TEncodedImage;
var
  WebImage: TInternalCastleWebImage;
  DetectedClass: TEncodedImageClass;
begin
  { Unless exactly 1 image classes is allowed,
    try to detect best class using format-specific DetectClass function.
    This may avoid the need for time-consuming Detect(...) call in CreateFromWebImage.
    Note that we don't limit this detection only to AllowedImageClasses = [] case,
    as LoadImage is often called with TextureImageClasses or TextureImageClassesAll. }
  DetectedClass := nil;
  if (High(AllowedImageClasses) <> 0) and
     Assigned(ImageFormatInfos[ImageFormat].DetectClass) then
  begin
    DetectedClass := ImageFormatInfos[ImageFormat].DetectClass(Stream);
    if (DetectedClass <> nil) and
       (not ClassAllowed(DetectedClass, AllowedImageClasses)) then
    begin
      DetectedClass := nil; // do not use
    end;
    // for debug:
    // if DetectedClass <> nil then
    //   WritelnLog('Will use detected image class %s (format %s)', [
    //     DetectedClass.ClassName,
    //     ImageFormatInfos[ImageFormat].FormatName
    //   ]);
  end;

  WebImage := CreateWebImage(0, 0);
  try
    try
      WebImage.LoadFromStream(Stream);
    except
      { WebImage.LoadFromStream may raise exception if it cannot decode image }
      on E: Exception do
        raise EImageLoadError.Create(E.Message)
    end;

    if DetectedClass <> nil then
      Result := CreateFromWebImage(WebImage, [DetectedClass])
    else
      Result := CreateFromWebImage(WebImage, AllowedImageClasses);
  finally
    FreeAndNil(WebImage);
  end;
end;

{ TEncodedImage -> TInternalCastleWebImage --------------------------------------------- }

function TEncodedImage.ToWebImage: TInternalCastleWebImage;
begin
  raise EImageSaveError.Create('Cannot convert to WebImage the images of class ' + ClassName);
  Result := nil; // avoid warning
end;

function TGrayscaleImage.ToWebImage: TInternalCastleWebImage;
var
  X, Y: Integer;
  P: PByte;
  Color: TWebColorRGBAValue;
begin
  Result := CreateWebImage(Width, Height);
  try
    P := Pixels;
    for Y := Result.Height - 1 downto 0 do
      for X := 0 to Result.Width - 1 do
      begin
        Color.R := P^;
        Color.G := Color.R;
        Color.B := Color.R;
        Color.A := $FF;
        Result.Colors[X, Y] := Color;
        Inc(P);
      end;
  except FreeAndNil(Result); raise end;
end;

function TGrayscaleAlphaImage.ToWebImage: TInternalCastleWebImage;
var
  X, Y: Integer;
  P: PVector2Byte;
  Color: TWebColorRGBAValue;
begin
  Result := CreateWebImage(Width, Height);
  try
    P := Pixels;
    for Y := Result.Height - 1 downto 0 do
      for X := 0 to Result.Width - 1 do
      begin
        Color.R := P^.Data[0];
        Color.G := Color.R;
        Color.B := Color.R;
        Color.A := P^.Data[1];
        Result.Colors[X, Y] := Color;
        Inc(P);
      end;
  except FreeAndNil(Result); raise end;
end;

function TRGBImage.ToWebImage: TInternalCastleWebImage;
var
  X, Y: Integer;
  P: PVector3Byte;
  Color: TWebColorRGBAValue;
begin
  Result := CreateWebImage(Width, Height);
  try
    P := Pixels;
    for Y := Result.Height - 1 downto 0 do
      for X := 0 to Result.Width - 1 do
      begin
        Color.R := P^.Data[0];
        Color.G := P^.Data[1];
        Color.B := P^.Data[2];
        Color.A := $FF;
        Result.Colors[X, Y] := Color;
        Inc(P);
      end;
  except FreeAndNil(Result); raise end;
end;

function TRGBAlphaImage.ToWebImage: TInternalCastleWebImage;
var
  X, Y: Integer;
  P: PVector4Byte;
  Color: TWebColorRGBAValue;
begin
  Result := CreateWebImage(Width, Height);
  try
    P := Pixels;
    for Y := Result.Height - 1 downto 0 do
      for X := 0 to Result.Width - 1 do
      begin
        Color.R := P^.Data[0];
        Color.G := P^.Data[1];
        Color.B := P^.Data[2];
        Color.A := P^.Data[3];
        Result.Colors[X, Y] := Color;
        Inc(P);
      end;
  except FreeAndNil(Result); raise end;
end;

function TRGBFloatImage.ToWebImage: TInternalCastleWebImage;
var
  X, Y: Integer;
  P: PVector3;
  Col: TVector3Byte;
  Color: TWebColorRGBAValue;
begin
  Result := CreateWebImage(Width, Height);
  try
    P := Pixels;
    for Y := Result.Height - 1 downto 0 do
      for X := 0 to Result.Width - 1 do
      begin
        Col := Vector3Byte(P^);
        Color.R := Col.Data[0];
        Color.G := Col.Data[1];
        Color.B := Col.Data[2];
        Color.A := $FF;
        Result.Colors[X, Y] := Color;
        Inc(P);
      end;
  except FreeAndNil(Result); raise end;
end;

procedure Save_WebImage(Img: TEncodedImage; Stream: TStream);
begin
  raise EImageSaveError.Create('Web target does not support saving image');
end;

{$endif} // defined(FPC) and defined(FPC_WASI)