{
  Copyright 2024-2024 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Main view, where most of the application logic takes place. }
unit GameViewMain;

interface

uses Classes,
  CastleVectors, CastleComponentSerialize, CastleScene, CastleViewport,
  CastleUIControls, CastleControls, CastleKeysMouse, CastleIfc,
  CastleCameras, CastleTransform, CastleTransformManipulate;

type
  { Main view, where most of the application logic takes place. }
  TViewMain = class(TCastleView)
  published
    { Components designed using CGE editor.
      These fields will be automatically initialized at Start. }
    LabelFps, LabelWireframeEffect, LabelHierarchy: TCastleLabel;
    ButtonNew, ButtonLoad, ButtonSaveIfc, ButtonSaveNode,
      ButtonAddWall, ButtonModifyRandomElement, ButtonChangeWireframeEffect: TCastleButton;
    IfcScene: TCastleScene;
    MainViewport: TCastleViewport;
    ExamineNavigation: TCastleExamineNavigation;
    TransformSelectedProduct: TCastleTransform;
  private
    IfcFile: TIfcFile;
    { New elements (TIfcElement instances) have to be added to this container.
      You cannot just add them to IfcFile.Project,
      IFC specification constaints what can be the top-level spatial element. }
    IfcContainer: TIfcSpatialElement;
    IfcMapping: TCastleIfcMapping;

    { Selected IFC product. }
    IfcSelectedProduct: TIfcProduct;
    { Value of TransformSelectedProduct.Translation
      that corresponds to the IfcSelectedProduct current position.
      Only meaningful when IfcSelectedProduct <> nil, undefined and unused otherwise. }
    IfcSelectedProductShapeTranslation: TVector3;

    { Used to assing unique names to walls. }
    NextWallNumber: Cardinal;

    //TransformHover: TCastleTransformHover; //< TODO, show hover
    TransformManipulate: TCastleTransformManipulate;

    { Create new IfcMapping instance and update what IfcScene shows,
      based on IfcFile contents.
      Use this after completely changing the IfcFile contents
      (like loading new file, or creating new file). }
    procedure NewIfcMapping(const NewIfcFile: TIfcFile);

    { Update LabelHierarchy.Caption to debug state of IfcFile. }
    procedure UpdateLabelHierarchy;

    procedure ClickNew(Sender: TObject);
    procedure ClickLoad(Sender: TObject);
    procedure ClickSaveIfc(Sender: TObject);
    procedure ClickSaveNode(Sender: TObject);
    procedure ClickAddWall(Sender: TObject);
    procedure ClickModifyRandomElement(Sender: TObject);
    procedure ClickChangeWireframeEffect(Sender: TObject);
    procedure MainViewportPress(const Sender: TCastleUserInterface;
      const Event: TInputPressRelease; var Handled: Boolean);
    procedure TransformManipulateTransformModified(Sender: TObject);
  public
    constructor Create(AOwner: TComponent); override;
    procedure Start; override;
    procedure Stop; override;
    procedure Update(const SecondsPassed: Single; var HandleInput: Boolean); override;
  end;

var
  ViewMain: TViewMain;

implementation

uses SysUtils, TypInfo,
  CastleUtils, CastleUriUtils, CastleWindow, CastleBoxes, X3DLoad, CastleLog,
  CastleRenderOptions, CastleShapes, X3DNodes;

function WireframeEffectToStr(const WireframeEffect: TWireframeEffect): String;
begin
  Result := GetEnumName(TypeInfo(TWireframeEffect), Ord(WireframeEffect));
end;

{ TViewMain ----------------------------------------------------------------- }

constructor TViewMain.Create(AOwner: TComponent);
begin
  inherited;
  DesignUrl := 'castle-data:/gameviewmain.castle-user-interface';
end;

procedure TViewMain.Start;
begin
  inherited;

  TransformManipulate := TCastleTransformManipulate.Create(FreeAtStop);
  TransformManipulate.Mode := mmTranslate;
  TransformManipulate.OnTransformModified := {$ifdef FPC}@{$endif} TransformManipulateTransformModified;

  { Initialize empty model.
    TransformManipulate must be set earlier. }
  ClickNew(nil);

  ButtonNew.OnClick := {$ifdef FPC}@{$endif} ClickNew;
  ButtonLoad.OnClick := {$ifdef FPC}@{$endif} ClickLoad;
  ButtonSaveIfc.OnClick := {$ifdef FPC}@{$endif} ClickSaveIfc;
  ButtonSaveNode.OnClick := {$ifdef FPC}@{$endif} ClickSaveNode;
  ButtonAddWall.OnClick := {$ifdef FPC}@{$endif} ClickAddWall;
  ButtonModifyRandomElement.OnClick := {$ifdef FPC}@{$endif} ClickModifyRandomElement;
  ButtonChangeWireframeEffect.OnClick := {$ifdef FPC}@{$endif} ClickChangeWireframeEffect;

  MainViewport.OnPress := {$ifdef FPC}@{$endif} MainViewportPress;

  LabelWireframeEffect.Caption := WireframeEffectToStr(IfcScene.RenderOptions.WireframeEffect);

  { Rotate by dragging with right mouse button,
    because we use left mouse button for selection. }
  ExamineNavigation.Input_Rotate.MouseButton := buttonRight;
  ExamineNavigation.Input_Zoom.MouseButtonUse := false;

  // TODO: show on hover
  // TransformHover := TCastleTransformHover.Create(FreeAtStop);
  // TransformHover.Current := TransformSelectedProduct;
end;

procedure TViewMain.Stop;
begin
  FreeAndNil(IfcFile);
  FreeAndNil(IfcMapping);
  inherited;
end;

procedure TViewMain.Update(const SecondsPassed: Single; var HandleInput: Boolean);
begin
  inherited;
  { This virtual method is executed every frame (many times per second). }
  Assert(LabelFps <> nil, 'If you remove LabelFps from the design, remember to remove also the assignment "LabelFps.Caption := ..." from code');
  LabelFps.Caption := 'FPS: ' + Container.Fps.ToString;
end;

procedure TViewMain.NewIfcMapping(const NewIfcFile: TIfcFile);
begin
  FreeAndNil(IfcMapping);

  IfcMapping := TCastleIfcMapping.Create;
  { The 'castle-data:/' below will be used as base URL to resolve any texture
    URLs inside IFC. As they are not possible in this demo for now,
    this value doesn't really matter. }
  IfcMapping.Load(IfcFile, 'castle-data:/');

  IfcScene.Load(IfcMapping.RootNode, true);

  UpdateLabelHierarchy;

  IfcSelectedProduct := nil;
  TransformManipulate.SetSelected([]);
end;

procedure TViewMain.ClickNew(Sender: TObject);

  procedure AddFloor;
  const
    FloorSize = 10;
  var
    Slab: TIfcSlab;
  begin
    Slab := TIfcSlab.Create(IfcFile);
    Slab.Name := 'Floor';
    Slab.PredefinedType := TIfcSlabTypeEnum.Floor;
    Slab.AddMeshRepresentation(IfcFile.Project.ModelContext, [
      Vector3(-FloorSize / 2, -FloorSize / 2, 0),
      Vector3( FloorSize / 2, -FloorSize / 2, 0),
      Vector3( FloorSize / 2,  FloorSize / 2, 0),
      Vector3(-FloorSize / 2,  FloorSize / 2, 0)
    ], [0, 1, 2, 3]);

    IfcContainer.AddContainedElement(Slab);

    { Note: one needs to call IfcMapping.Update(IfcFile) after the changes.
      But in this case, we call NewIfcMapping after AddFloor,
      so no need for extra update. }
  end;

var
  IfcSite: TIfcSite;
  IfcBuilding: TIfcBuilding;
  IfcBuildingStorey: TIfcBuildingStorey;
begin
  FreeAndNil(IfcFile); // owns all other IFC classes, so this frees everything

  IfcFile := TIfcFile.Create(nil);

  { Obligatory initialization of new IFC files structure.
    We must set IfcFile.Project,
    each project must have units,
    and each project must have a 3D model context. }
  IfcFile.Project := TIfcProject.Create(IfcFile);
  IfcFile.Project.SetupUnits;
  IfcFile.Project.SetupModelContext;

  { We need IfcContainer inside the project.
    Reason: We cannot add elements directly to IfcProject, we need
    to add them to a spatial root element: (IfcSite || IfcBuilding || IfcSpatialZone)
    (see https://standards.buildingsmart.org/IFC/RELEASE/IFC4_3/HTML/lexical/IfcProject.htm ).

    The IfcSpatialStructureElement
    (see https://standards.buildingsmart.org/IFC/RELEASE/IFC4_3/HTML/lexical/IfcSpatialStructureElement.htm )
    defines the hierarchy of root classes, and implies that root hierarchy
    can be (one case, seems most common -- BlenderBIM also follows it):

    - IfcSite inside IfcProject,
    - with IfcBuilding inside,
    - with IfcBuildingStorey inside,
    inserted into each other using "is composed by" relationship. }

  IfcSite := TIfcSite.Create(IfcFile);
  IfcSite.Name := 'My Site';
  IfcFile.Project.AddIsDecomposedBy(IfcSite);

  IfcBuilding := TIfcBuilding.Create(IfcFile);
  IfcBuilding.Name := 'My Building';
  IfcSite.AddIsDecomposedBy(IfcBuilding);

  IfcBuildingStorey := TIfcBuildingStorey.Create(IfcFile);
  IfcBuildingStorey.Name := 'My Building Storey';
  IfcBuilding.AddIsDecomposedBy(IfcBuildingStorey);

  IfcContainer := IfcBuildingStorey;

  AddFloor;

  NewIfcMapping(IfcFile);
end;

const
  IfcFileFilter = 'IFC JSON (*.ifcjson)|*.ifcjson|All Files|*';

procedure TViewMain.ClickLoad(Sender: TObject);
var
  Url: string;
  IfcSite: TIfcSite;
  IfcBuilding: TIfcBuilding;
  IfcBuildingStorey: TIfcBuildingStorey;
begin
  Url := 'castle-data:/';
  if Application.MainWindow.FileDialog('Load IFC file', Url, true, IfcFileFilter) then
  begin
    FreeAndNil(IfcFile);
    IfcFile := IfcJsonLoad(Url);

    // initialize IfcContainer, needed as parent for new walls
    IfcContainer := IfcFile.Project.BestContainer;
    if IfcContainer = nil then
    begin
      WritelnWarning('IFC model "%s" is missing a spatial root element (IfcSite, IfcBuilding, IfcBuildingStorey), adding a dummy one', [
        Url
      ]);

      IfcSite := TIfcSite.Create(IfcFile);
      IfcSite.Name := 'My Site';
      IfcFile.Project.AddIsDecomposedBy(IfcSite);

      IfcBuilding := TIfcBuilding.Create(IfcFile);
      IfcBuilding.Name := 'My Building';
      IfcSite.AddIsDecomposedBy(IfcBuilding);

      IfcBuildingStorey := TIfcBuildingStorey.Create(IfcFile);
      IfcBuildingStorey.Name := 'My Building Storey';
      IfcBuilding.AddIsDecomposedBy(IfcBuildingStorey);

      IfcContainer := IfcBuildingStorey;
    end;

    WritelnLog('IFC best container in "%s" guessed as "%s"', [
      Url,
      IfcContainer.ClassName
    ]);

    // make sure we have ModelContext to add new walls
    if IfcFile.Project.ModelContext = nil then
    begin
      WritelnWarning('IFC model "%s" is missing a "Model" context in project.representationContexts, this can happen for IFC exported from BonsaiBIM', [
        Url
      ]);
      IfcFile.Project.SetupModelContext;
    end;
    NewIfcMapping(IfcFile);
  end;
end;

procedure TViewMain.ClickSaveIfc(Sender: TObject);
var
  Url: string;
begin
  Url := 'castle-data:/out.ifcjson';
  if Application.MainWindow.FileDialog('Save IFC file',
      Url, false, IfcFileFilter) then
  begin
    IfcJsonSave(IfcFile, Url);
  end;
end;

procedure TViewMain.ClickSaveNode(Sender: TObject);
var
  Url: string;
begin
  Url := 'castle-data:/out.x3d';
  if Application.MainWindow.FileDialog('Save Node To X3D or STL',
      Url, false, SaveNode_FileFilters) then
  begin
    SaveNode(IfcMapping.RootNode, Url);
  end;
end;

procedure TViewMain.ClickAddWall(Sender: TObject);
var
  Wall: TIfcWall;
  SizeX, SizeY, WallHeight: Single;
begin
  Wall := TIfcWall.Create(IfcFile);
  Wall.Name := 'Wall' + IntToStr(NextWallNumber);
  Inc(NextWallNumber);

  SizeX := RandomFloatRange(1, 4);
  SizeY := RandomFloatRange(1, 4);
  WallHeight := RandomFloatRange(1.5, 2.5); // Z is "up", by convention, in IFC
  Wall.AddBoxRepresentation(IfcFile.Project.ModelContext,
    Box3D(
      Vector3(-SizeX / 2, -SizeY / 2, 0),
      Vector3( SizeX / 2,  SizeY / 2, WallHeight)
    ));

  Wall.RelativePlacement := Vector3(
    RandomFloatRange(-5, 5),
    RandomFloatRange(-5, 5),
    0
  );

  IfcContainer.AddContainedElement(Wall);
  IfcMapping.Update(IfcFile);
  UpdateLabelHierarchy;
end;

procedure TViewMain.ClickModifyRandomElement(Sender: TObject);
var
  ElementList: TIfcElementList;
  RandomElement: TIfcElement;
begin
  ElementList := TIfcElementList.Create(false);
  try
    IfcContainer.GetContainedElements(ElementList);
    if ElementList.Count = 0 then
      Exit;
    RandomElement := ElementList[Random(ElementList.Count)];
    if RandomElement.PlacementRelativeToParent then
    begin
      RandomElement.RelativePlacement := Vector3(
        RandomFloatRange(-5, 5),
        RandomFloatRange(-5, 5),
        0
      );
      WritelnLog('Modified element "%s" placement', [RandomElement.Name]);
    end else
      WritelnWarning('Element "%s" is not positioned relative to parent, cannot modify', [
        RandomElement.Name
      ]);
  finally FreeAndNil(ElementList) end;

  IfcMapping.Update(IfcFile);

  { IfcSelectedProductShapeTranslation is no longer valid because
    IfcSelectedProduct moved, so cancel selection. }
  if RandomElement = IfcSelectedProduct then
  begin
    IfcSelectedProduct := nil;
    TransformManipulate.SetSelected([]);
  end;
end;

procedure TViewMain.ClickChangeWireframeEffect(Sender: TObject);
begin
  if IfcScene.RenderOptions.WireframeEffect = High(TWireframeEffect) then
    IfcScene.RenderOptions.WireframeEffect := Low(TWireframeEffect)
  else
    IfcScene.RenderOptions.WireframeEffect := Succ(IfcScene.RenderOptions.WireframeEffect);
  LabelWireframeEffect.Caption := WireframeEffectToStr(IfcScene.RenderOptions.WireframeEffect);
end;

procedure TViewMain.UpdateLabelHierarchy;
const
  Indent = '  ';
var
  SList: TStringList;

  procedure ShowHierarchy(const Parent: TIfcObjectDefinition; const NowIndent: String);
  var
    ParentSpatial: TIfcSpatialElement;
    RelAggregates: TIfcRelAggregates;
    RelatedObject: TIfcObjectDefinition;
    RelContained: TIfcRelContainedInSpatialStructure;
    RelatedProduct: TIfcProduct;
    S: String;
  begin
    S := NowIndent + Parent.ClassName + ' "' + Parent.Name + '"';
    if Parent = IfcSelectedProduct then
      S := '<font color="#ffff00">' + S + '</font>';
    SList.Add(S);

    if Parent = IfcFile.Project.BestContainer then
      SList.Add(NowIndent + Indent + '<font color="#0000aa">(^detected best container)</font>');
    if (Parent is TIfcProduct) and
       (not TIfcProduct(Parent).PlacementRelativeToParent) then
      SList.Add(NowIndent + Indent + '<font color="#aa0000">(^cannot drag placement)</font>');

    for RelAggregates in Parent.IsDecomposedBy do
      for RelatedObject in RelAggregates.RelatedObjects do
        ShowHierarchy(RelatedObject, NowIndent + Indent);

    if Parent is TIfcSpatialElement then
    begin
      ParentSpatial := TIfcSpatialElement(Parent);
      for RelContained in ParentSpatial.ContainsElements do
        for RelatedProduct in RelContained.RelatedElements do
          ShowHierarchy(RelatedProduct, NowIndent + Indent);
    end;
  end;

var
  RepresentationContext: TIfcRepresentationContext;
begin
  { Showing the IFC hierarchy, as seen by Castle Game Engine,
    is a useful debug tool (both for CGE and for your IFC models).
    We show various relations (like IsDecomposedBy, ContainsElements),
    we show the representation contexts, we mark what was detected
    as Project.ModelContext, Project.PlanContext, Project.BestContainer. }

  SList := TStringList.Create;
  try
    SList.Add('<font color="#008800">Representation contexts:</font>');

    for RepresentationContext in IfcFile.Project.RepresentationContexts do
    begin
      SList.Add(Indent + RepresentationContext.ClassName);
      if RepresentationContext = IfcFile.Project.ModelContext then
        SList.Add(Indent + Indent + '<font color="#0000aa">(^detected 3D ModelContext)</font>');
      if RepresentationContext = IfcFile.Project.PlanContext then
        SList.Add(Indent + Indent + '<font color="#0000aa">(^detected 2D PlanContext)</font>');
    end;

    SList.Add('');
    SList.Add('<font color="#008800">Project Hierarchy:</font>');
    ShowHierarchy(IfcFile.Project, Indent);

    LabelHierarchy.Text.Assign(SList);
  finally FreeAndNil(SList) end;
end;

procedure TViewMain.MainViewportPress(const Sender: TCastleUserInterface;
  const Event: TInputPressRelease; var Handled: Boolean);
var
  HitInfo: TRayCollisionNode;
  HitShape: TShape;
  HitShapeNode: TAbstractShapeNode;
  NewSelectedProduct: TIfcProduct;
begin
  if Event.IsMouseButton(buttonLeft) then
  begin
    { Select new IFC product by extracting from MainViewport.MouseRayHit
      information the selected X3D shape (HitShape) and then converting it
      to the IFC product (using IfcMapping.NodeToProduct). }
    if (MainViewport.MouseRayHit <> nil) and
       MainViewport.MouseRayHit.Info(HitInfo) then
    begin
      if (HitInfo.Item = IfcScene) and
         (HitInfo.Triangle <> nil) and
          (HitInfo.Triangle^.ShapeNode <> nil) then
      begin
        HitShape := HitInfo.Triangle^.Shape;
        HitShapeNode := HitShape.Node;
        NewSelectedProduct := IfcMapping.NodeToProduct(HitShapeNode);
      end else
        { If we hit something, but it's not IFC scene, abort handling
          this click. This may be a start of dragging on TransformManipulate,
          we don't want to interfere with it. }
        Exit;
    end else
      NewSelectedProduct := nil;

    if NewSelectedProduct <> IfcSelectedProduct then
    begin
      IfcSelectedProduct := NewSelectedProduct;
      UpdateLabelHierarchy;

      // update TransformManipulate, to allow dragging selected product
      if (IfcSelectedProduct <> nil) and
         IfcSelectedProduct.PlacementRelativeToParent and
         (not HitShape.BoundingBox.IsEmpty) then
      begin
        TransformSelectedProduct.Translation := HitShape.BoundingBox.Center;
        IfcSelectedProductShapeTranslation := TransformSelectedProduct.Translation;
        TransformManipulate.SetSelected([TransformSelectedProduct]);
      end else
        TransformManipulate.SetSelected([]);
    end;

    Handled := true;
  end;
end;

procedure TViewMain.TransformManipulateTransformModified(Sender: TObject);
begin
  Assert(IfcSelectedProduct <> nil, 'TransformManipulateTransformModified called without IfcSelectedProduct, should not happen');

  { Update IfcSelectedProduct.RelativePlacement based on
    difference in shift of TransformSelectedProduct. Note that this simple way
    to update translation assumes we don't have rotations or scale in IFC. }
  IfcSelectedProduct.RelativePlacement :=
    IfcSelectedProduct.RelativePlacement +
    TransformSelectedProduct.Translation -
    IfcSelectedProductShapeTranslation;
  IfcMapping.Update(IfcFile);
  IfcSelectedProductShapeTranslation := TransformSelectedProduct.Translation;
end;

end.
