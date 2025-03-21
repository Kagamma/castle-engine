{ -*- buffer-read-only: t -*-

  Copyright 2015-2025 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Automatically generated node properties.

  Do not edit this file manually!
  To add new properties:
  - add them to the text files in tools/internal/x3d-nodes-to-pascal/nodes-specification/ ,
  - and regenerate include files by running x3d-nodes-to-pascal }

{$ifdef read_interface}

  strict private
    function GetAlpha: Single;
    procedure SetAlpha(const Value: Single);
    function GetColor: TCastleColorRGB;
    procedure SetColor(const Value: TCastleColorRGB);
  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FFdAlpha: TSFFloat;
    { Internal wrapper for property @code(Alpha). This wrapper API may change, we advise to access simpler @code(Alpha) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdAlpha: TSFFloat read FFdAlpha;
    {  }
    property Alpha: Single read GetAlpha write SetAlpha;

    strict private FFdColor: TSFColor;
    { Internal wrapper for property @code(Color). This wrapper API may change, we advise to access simpler @code(Color) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdColor: TSFColor read FFdColor;
    { Avoid Delphi warning that this hides ancestor Color.
      We don't care about it now -- because ancestor's Color
      method is not very useful on TMultiTextureNode anyway.
      TODO: Make this persistent across regenerations of this include. }
    {$warnings off}
    {  }
    property Color: TCastleColorRGB read GetColor write SetColor;
    {$warnings on}

    strict private FFdFunction: TMFString;
    { Internal wrapper for property @code(Function). This wrapper API may change, we advise to access simpler @code(Function) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdFunction: TMFString read FFdFunction;
    {  }
    procedure SetFunction(const Value: array of string); overload;
    {  }
    procedure SetFunction(const Value: TCastleStringList); overload;

    strict private FFdMode: TMFString;
    { Internal wrapper for property @code(Mode). This wrapper API may change, we advise to access simpler @code(Mode) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdMode: TMFString read FFdMode;
    {  }
    procedure SetMode(const Value: array of string); overload;
    {  }
    procedure SetMode(const Value: TCastleStringList); overload;

    strict private FFdSource: TMFString;
    { Internal wrapper for property @code(Source). This wrapper API may change, we advise to access simpler @code(Source) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdSource: TMFString read FFdSource;
    {  }
    procedure SetSource(const Value: array of string); overload;
    {  }
    procedure SetSource(const Value: TCastleStringList); overload;

    strict private FFdTexture: TMFNode;
    { Internal wrapper for property @code(Texture). This wrapper API may change, we advise to access simpler @code(Texture) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdTexture: TMFNode read FFdTexture;
    {  }
    procedure SetTexture(const Value: array of TAbstractSingleTextureNode);

{$endif read_interface}

{$ifdef read_implementation}

{ TMultiTextureNode ----------------------------------------------- }

function TMultiTextureNode.GetAlpha: Single;
begin
  Result := FdAlpha.Value;
end;

procedure TMultiTextureNode.SetAlpha(const Value: Single);
begin
  FdAlpha.Send(Value);
end;

function TMultiTextureNode.GetColor: TCastleColorRGB;
begin
  Result := FdColor.Value;
end;

procedure TMultiTextureNode.SetColor(const Value: TCastleColorRGB);
begin
  FdColor.Send(Value);
end;

procedure TMultiTextureNode.SetFunction(const Value: array of string);
begin
  FdFunction.Send(Value);
end;

procedure TMultiTextureNode.SetFunction(const Value: TCastleStringList);
begin
  FdFunction.Send(Value);
end;

procedure TMultiTextureNode.SetMode(const Value: array of string);
begin
  FdMode.Send(Value);
end;

procedure TMultiTextureNode.SetMode(const Value: TCastleStringList);
begin
  FdMode.Send(Value);
end;

procedure TMultiTextureNode.SetSource(const Value: array of string);
begin
  FdSource.Send(Value);
end;

procedure TMultiTextureNode.SetSource(const Value: TCastleStringList);
begin
  FdSource.Send(Value);
end;

procedure TMultiTextureNode.SetTexture(const Value: array of TAbstractSingleTextureNode);
var
  L: Integer;
  A: array of TX3DNode;
begin
  L := High(Value) + 1;
  SetLength(A, L);
  if L > 0 then
    Move(Value[0], A[0], L * SizeOf(TAbstractSingleTextureNode));
  FdTexture.Send(A);
end;

class function TMultiTextureNode.ClassX3DType: String;
begin
  Result := 'MultiTexture';
end;

procedure TMultiTextureNode.CreateNode;
begin
  inherited;

  FFdAlpha := TSFFloat.Create(Self, True, 'alpha', 1);
   FdAlpha.ChangeAlways := chTextureRendererProperties;
   FdAlpha.MustBeNonnegative := true;
  AddField(FFdAlpha);

  FFdColor := TSFColor.Create(Self, True, 'color', Vector3(1, 1, 1));
   FdColor.ChangeAlways := chTextureRendererProperties;
  AddField(FFdColor);

  FFdFunction := TMFString.Create(Self, True, 'function', []);
   FdFunction.ChangeAlways := chTextureRendererProperties;
  AddField(FFdFunction);

  FFdMode := TMFString.Create(Self, True, 'mode', []);
   FdMode.ChangeAlways := chTextureRendererProperties;
  AddField(FFdMode);

  FFdSource := TMFString.Create(Self, True, 'source', []);
   FdSource.ChangeAlways := chTextureRendererProperties;
  AddField(FFdSource);

  FFdTexture := TMFNode.Create(Self, True, 'texture', [TAbstractSingleTextureNode]);
   FdTexture.ChangeAlways := chTextureRendererProperties;
  AddField(FFdTexture);
end;

{$endif read_implementation}
