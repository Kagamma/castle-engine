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

{ Manage Web context }

{$ifdef read_application_interface}
private
  var
    FScreenWidth, FScreenHeight: Integer;
{$endif}

{$ifdef read_implementation}
type
  TQueuedEventType = (aeResize, aeKeyDown, aeKeyUp, aeMouseMove, aeMouseDown, aeMouseUp, aeMouseWheel);
  TQueuedEvent = object
    EventType: TQueuedEventType;
    Key: TKey;
    Value1, Value2: Integer;
    MouseButton: TCastleMouseButton;
  end;
  TQueuedEventList = specialize TQueue<TQueuedEvent>;

var
  QEvents: TQueuedEventList;

{ Externals }

procedure SetPageCaption(S: PChar); external 'window' name 'setCaption';

{ TCastleWindowBase ---------------------------------------------------------- }

procedure TCastleWindowBase.CreateBackend;
begin
end;

procedure TCastleWindowBase.BackendMenuInitialize;
begin
end;

procedure TCastleWindowBase.BackendMenuFinalize;
begin
end;

procedure TCastleWindowBase.BackendMakeCurrent;
begin
end;

procedure TCastleWindowBase.SwapBuffers;
begin
end;

function TCastleWindowBase.BackendFileDialog(const Title: String; var FileName: String;
  OpenDialog: Boolean; FileFilters: TFileFilterList = nil): Boolean;
begin
end;

function TCastleWindowBase.ColorDialog(var Color: TVector3): Boolean;
begin
end;

procedure TCastleWindowBase.MessageOK(const S: String; const MessageType: TWindowMessageType);
begin
end;

function TCastleWindowBase.MessageYesNo(const S: String;
  const MessageType: TWindowMessageType): Boolean;
begin
end;

procedure TCastleWindowBase.MenuUpdateCaption(Entry: TMenuEntryWithCaption);
begin
end;

procedure TCastleWindowBase.MenuUpdateEnabled(Entry: TMenuEntryWithCaption);
begin
end;

procedure TCastleWindowBase.MenuUpdateChecked(Entry: TMenuItemChecked);
begin
end;

function TCastleWindowBase.MenuUpdateCheckedFast: Boolean;
begin
end;

procedure TCastleWindowBase.MenuInsert(const Parent: TMenu;
  const ParentPosition: Integer; const Entry: TMenuEntry);
begin
end;

procedure TCastleWindowBase.MenuDelete(const Parent: TMenu;
  const ParentPosition: Integer; const Entry: TMenuEntry);
begin
end;

procedure TCastleWindowBase.OpenBackend;
begin
end;

procedure TCastleWindowBase.CloseBackend;
begin
end;

procedure TCastleWindowBase.SetCaption(const Part: TCaptionPart; const Value: String);
begin
  FCaption[Part] := Value;
  SetPageCaption(PChar(Value));
end;

procedure TCastleWindowBase.SetCursor(const Value: TMouseCursor);
begin
  if FCursor <> Value then
  begin
    FCursor := Value;
    if not Closed then
      { TODO UpdateCursor };
  end;
end;

procedure TCastleWindowBase.SetCustomCursor(const Value: TRGBAlphaImage);
begin
  { TODO }
  FCustomCursor := Value;
end;

function TCastleWindowBase.RedirectKeyDownToMenuClick: Boolean;
begin
end;

procedure TCastleWindowBase.SetMousePosition(const Value: TVector2);
begin
  if not Closed then
    FMousePosition := Value;
end;

procedure TCastleWindowBase.UpdateFullScreenBackend;
begin
  FFullScreenBackend := FFullScreenWanted; // does nothing on web
end;

{ TCastleApplication ---------------------------------------------------------- }

procedure TCastleApplication.CreateBackend;
begin
  QEvents := TQueuedEventList.Create;
end;

procedure TCastleApplication.DestroyBackend;
begin
  FreeAndNil(QEvents);
end;

function TCastleApplication.ProcessAllMessages: Boolean;
begin
  Result := ProcessMessage(false, false);
end;

function TCastleApplication.ProcessMessage(WaitForMessage, WaitToLimitFPS: Boolean): Boolean;

  procedure WindowsUpdateAndTimer;
  begin
    DoApplicationUpdate;
    FOpenWindows.DoUpdate;
    MaybeDoTimer;
  end;

var
  QEvent: TQueuedEvent;
  MessageHandlingBegin: TTimerResult;
begin
  MessageHandlingBegin := Timer;
  repeat
    while QEvents.Count > 0 do
    begin
      QEvent := QEvents.Dequeue;
      case QEvent.EventType of
        aeResize:
          begin
            FScreenWidth := QEvent.Value1;
            FScreenHeight := QEvent.Value2;
            if not MainWindow.Closed then
              MainWindow.DoResize(FScreenWidth, FScreenHeight, false);
          end;
        aeKeyDown:
          begin
            if not MainWindow.Closed then
              MainWindow.DoKeyDown(QEvent.Key, Char(QEvent.Key));
          end;
        aeKeyUp:
          begin
            if not MainWindow.Closed then
              MainWindow.DoKeyUp(QEvent.Key);
          end;
        aeMouseMove:
          begin
            if not MainWindow.Closed then
              MainWindow.DoMotion(InputMotion(MainWindow.MousePosition, Vector2(QEvent.Value1, QEvent.Value2), MainWindow.MousePressed, 0));
          end;
        aeMouseDown:
          begin
            if not MainWindow.Closed then
              MainWindow.DoMouseDown(Vector2(QEvent.Value1, QEvent.Value2), QEvent.MouseButton, 0);
          end;
        aeMouseUp:
          begin
            if not MainWindow.Closed then
              MainWindow.DoMouseUp(Vector2(QEvent.Value1, QEvent.Value2), QEvent.MouseButton, 0);
          end;
        aeMouseWheel:
          begin
            if not MainWindow.Closed then
              MainWindow.DoMouseWheel(QEvent.Value1, true);
          end;
      end;
    end;
  // loop condition avoids being clogged with motion events
  until TimerSeconds(Timer, MessageHandlingBegin) > 1 / ApplicationProperties.LimitFPS;
  MainWindow.DoRender;
  WindowsUpdateAndTimer;
  Result := not Terminated;
end;

procedure TCastleApplication.Run;
begin
  DoRun;
end;

procedure TCastleApplication.QuitWhenNoOpenWindows;
begin
end;

function TCastleApplication.ScreenWidth: Integer;
begin
  Result := FScreenWidth;
end;

function TCastleApplication.ScreenHeight: Integer;
begin
  Result := FScreenHeight;
end;

function TCastleApplication.ScreenStatusBarScaledHeight: Cardinal;
begin
end;

function TCastleApplication.BackendName: String;
begin
  Result := 'Web';
end;

{ TWindowContainer ----------------------------------------------------------- }

function TWindowContainer.SettingMousePositionCausesMotion: Boolean;
begin
  Result := true;
end;

{ Events, will be called by web browser }

procedure EventResize(Width, Height: Integer);
var
  QEvent: TQueuedEvent;
begin
  QEvent.EventType := aeResize;
  QEvent.Value1 := Width;
  QEvent.Value2 := Height;
  QEvents.Enqueue(QEvent);
end;

procedure EventKeyDown(KeyCode: Integer);
var
  QEvent: TQueuedEvent;
begin
  QEvent.EventType := aeKeyDown;
  QEvent.Key := TKey(KeyCode);
  QEvents.Enqueue(QEvent);
end;

procedure EventKeyUp(KeyCode: Integer);
var
  QEvent: TQueuedEvent;
begin
  QEvent.EventType := aeKeyUp;
  QEvent.Key := TKey(KeyCode);
  QEvents.Enqueue(QEvent);
end;

procedure EventMouseMove(X, Y: Integer);
var
  QEvent: TQueuedEvent;
begin
  QEvent.EventType := aeMouseMove;
  QEvent.Value1 := X;
  QEvent.Value2 := Y;
  QEvents.Enqueue(QEvent);
end;

procedure EventMouseDown(Button, X, Y: Integer);
var
  QEvent: TQueuedEvent;
begin
  QEvent.EventType := aeMouseDown;
  QEvent.Value1 := X;
  QEvent.Value2 := Y;
  case Button of
    0: QEvent.MouseButton := buttonLeft;
    1: QEvent.MouseButton := buttonMiddle;
    2: QEvent.MouseButton := buttonRight;
  end;
  QEvents.Enqueue(QEvent);
end;

procedure EventMouseUp(Button, X, Y: Integer);
var
  QEvent: TQueuedEvent;
begin
  QEvent.EventType := aeMouseUp;
  QEvent.Value1 := X;
  QEvent.Value2 := Y;
  case Button of
    0: QEvent.MouseButton := buttonLeft;
    1: QEvent.MouseButton := buttonMiddle;
    2: QEvent.MouseButton := buttonRight;
  end;
  QEvents.Enqueue(QEvent);
end;

procedure EventMouseWheel(Y: Integer);
var
  QEvent: TQueuedEvent;
begin
  QEvent.EventType := aeMouseWheel;
  QEvent.Value1 := Y;
  QEvents.Enqueue(QEvent);
end;

{ Web browser will call this event for main loop }
procedure EventRun;
begin
  if Application <> nil then
    Application.Run;
end;

exports
  EventResize,
  EventKeyDown,
  EventKeyUp,
  EventMouseMove,
  EventMouseDown,
  EventMouseUp,
  EventRun;

{$endif read_implementation}