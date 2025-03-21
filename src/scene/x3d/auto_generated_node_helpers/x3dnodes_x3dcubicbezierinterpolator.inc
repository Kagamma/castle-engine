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

  public
    { Create node fields and events. }
    procedure CreateNode; override;
    class function ClassX3DType: String; override;

    strict private FFdControlPoints: TMFVec4f;
    { Internal wrapper for property @code(ControlPoints). This wrapper API may change, we advise to access simpler @code(ControlPoints) instead, if it is defined (TODO: for now, some field types do not have a simpler counterpart). }
    public property FdControlPoints: TMFVec4f read FFdControlPoints;
    {  }
    procedure SetControlPoints(const Value: array of TVector4); overload;
    {  }
    procedure SetControlPoints(const Value: TVector4List); overload;

{$endif read_interface}

{$ifdef read_implementation}

{ TAbstractCubicBezierInterpolatorNode ----------------------------------------------- }

procedure TAbstractCubicBezierInterpolatorNode.SetControlPoints(const Value: array of TVector4);
begin
  FdControlPoints.Send(Value);
end;

procedure TAbstractCubicBezierInterpolatorNode.SetControlPoints(const Value: TVector4List);
begin
  FdControlPoints.Send(Value);
end;

class function TAbstractCubicBezierInterpolatorNode.ClassX3DType: String;
begin
  Result := 'X3DCubicBezierInterpolator';
end;

procedure TAbstractCubicBezierInterpolatorNode.CreateNode;
begin
  inherited;

  FFdControlPoints := TMFVec4f.Create(Self, True, 'controlPoints', []);
   FdControlPoints.ChangeAlways := chVisibleNonGeometry;
  AddField(FFdControlPoints);
end;

{$endif read_implementation}
