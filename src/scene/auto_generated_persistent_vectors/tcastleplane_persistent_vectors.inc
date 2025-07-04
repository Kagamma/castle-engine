{ -*- buffer-read-only: t -*-

  Copyright 2018-2022 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Automatically generated wrapper to make vectors/colors persistent.

  Do not edit this file manually!
  Instead
  - edit tools/internal/generate-persistent-vectors/persistent_vectors_input.txt
  - and run generate-persistent-vectors in that directory.
}

{$ifdef read_interface_class}
strict private
  FSizePersistent: TCastleVector2Persistent;
  function GetSizeForPersistent: TVector2;
  procedure SetSizeForPersistent(const AValue: TVector2);
published
  { @link(Size) that can be visually edited in
    Castle Game Engine Editor, Lazarus and Delphi.
    Normal user code does not need to deal with this,
    instead read or write @link(Size) directly.

    @seealso Size }
  property SizePersistent: TCastleVector2Persistent read FSizePersistent ;
{$endif read_interface_class}

{$ifdef read_implementation_methods}
function TCastlePlane.GetSizeForPersistent: TVector2;
begin
  Result := Size;
end;

procedure TCastlePlane.SetSizeForPersistent(const AValue: TVector2);
begin
  Size := AValue;
end;
{$endif read_implementation_methods}

{$ifdef read_implementation_constructor}
  FSizePersistent := TCastleVector2Persistent.Create(nil);
  { Note that some classes (like TCastleVector3Persistent) call SetSubComponent(true)
    on themselves, for backward compatibility. But we should not depend on it,
    some classes (like TFloatRectanglePersistent) may not call it.
    The responsibility of calling "SetSubComponent(true)" should be on the owner
    in general (this is more functional, and more standard too). }
  FSizePersistent.SetSubComponent(true);
  FSizePersistent.InternalGetValue := {$ifdef FPC}@{$endif}GetSizeForPersistent;
  FSizePersistent.InternalSetValue := {$ifdef FPC}@{$endif}SetSizeForPersistent;
  FSizePersistent.InternalDefaultValue := Size; // current value is default
{$endif read_implementation_constructor}

{$ifdef read_implementation_destructor}
  FreeAndNil(FSizePersistent);
{$endif read_implementation_destructor}

