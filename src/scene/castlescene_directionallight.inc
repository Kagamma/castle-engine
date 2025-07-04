{%MainUnit castlescene.pas}
{
  Copyright 2022-2024 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Directional light. }

{$ifdef read_interface}

type
  { Directional light shines along a direction in 3D space, simulating
    a light source far away (like a sun).
    The direction is -Z (in local coordinates), simply rotate this
    transformation to change the lighting direction.
    The position of this transformation doesn't matter for lighting calculation
    (though it is useful to display the light gizmo at design-time).

    See @url(https://www.web3d.org/documents/specifications/19775-1/V3.3/Part01/components/lighting.html#DirectionalLight
    X3D DirectionalLight specification) for various details. }
  TCastleDirectionalLight = class(TCastlePunctualLight)
  strict private
    { Same thing as FLightNode, but with proper type. }
    FDirectionalLight: TDirectionalLightNode;

    FColor: TCastleColorRGB;
    FIntensity: Single;

    procedure SetColor(const Value: TCastleColorRGB);
    procedure SetIntensity(const Value: Single);
  public
    const
      DefaultIntensity = 1.0;

    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    function PropertySections(const PropertyName: String): TPropertySections; override;

    { Light color shining over surface. By default white. }
    property Color: TCastleColorRGB read FColor write SetColor;
  published
    { Intensity, multiplied by color. Can be anything >= 0.
      Intuitively, how bright is the light.

      Note that the physical interpretation of what the @italic(intensity)
      means for a directional light is a bit different than for
      TCastlePointLight and TCastleSpotLight.
      Directional light doesn't have a position, consequently it doesn't have
      attenuation. Following @url(https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md glTF lights specification),
      the directional light's intensity physical unit
      is illuminance in lux (lm/m2).

      For the above reason, different values of intensity are "reasonable"
      for directional light than for point and spot lights.
      By default directional light intensity is 1.0, unlike point and spot lights
      which have intensity 100.0 by default. }
    property Intensity: Single read FIntensity write SetIntensity {$ifdef FPC}default DefaultIntensity{$endif};

  {$define read_interface_class}
  {$I auto_generated_persistent_vectors/tcastledirectionallight_persistent_vectors.inc}
  {$undef read_interface_class}
  end;

{$endif read_interface}

{$ifdef read_implementation}

constructor TCastleDirectionalLight.Create(AOwner: TComponent);

  function BuildGizmo: TX3DRootNode;
  begin
    Result := TX3DRootNode.Create;
    Result.AddChildren(Gizmo.LinesShape([
      TVector3.Zero,
      DefaultCameraDirection * 10
    ]));
  end;

begin
  inherited;

  // defaults
  FIntensity := DefaultIntensity;
  FColor := WhiteRGB;

  // initialize light node
  FDirectionalLight := TDirectionalLightNode.Create;
  FDirectionalLight.Global := true;
  LightNode := FDirectionalLight;
  FPunctualLight := FDirectionalLight;

  // apply TCastleDirectionalLight defaults to FDirectionalLight
  FDirectionalLight.Color := FColor;
  FDirectionalLight.Intensity := FIntensity;

  // initialize Gizmo
  if Gizmo <> nil then
  begin
    if InternalCastleDesignData <> '' then
      Gizmo.SetIconUrl(InternalCastleDesignData + 'gizmos/light/light_directional.png');
    Gizmo.LoadVisualization(BuildGizmo);
  end;

  {$define read_implementation_constructor}
  {$I auto_generated_persistent_vectors/tcastledirectionallight_persistent_vectors.inc}
  {$undef read_implementation_constructor}
end;

destructor TCastleDirectionalLight.Destroy;
begin
  {$define read_implementation_destructor}
  {$I auto_generated_persistent_vectors/tcastledirectionallight_persistent_vectors.inc}
  {$undef read_implementation_destructor}
  inherited;
end;

function TCastleDirectionalLight.PropertySections(const PropertyName: String): TPropertySections;
begin
  if ArrayContainsString(PropertyName, [
      'ColorPersistent', 'Intensity'
     ]) then
    Result := [psBasic]
  else
    Result := inherited PropertySections(PropertyName);
end;

procedure TCastleDirectionalLight.SetColor(const Value: TCastleColorRGB);
begin
  if not TCastleColorRGB.PerfectlyEquals(FColor, Value) then
  begin
    FColor := Value;
    FDirectionalLight.Color := Value;
  end;
end;

procedure TCastleDirectionalLight.SetIntensity(const Value: Single);
begin
  if FIntensity <> Value then
  begin
    FIntensity := Value;
    FDirectionalLight.Intensity := Value;
  end;
end;

{$define read_implementation_methods}
{$I auto_generated_persistent_vectors/tcastledirectionallight_persistent_vectors.inc}
{$undef read_implementation_methods}

{$endif read_implementation}
