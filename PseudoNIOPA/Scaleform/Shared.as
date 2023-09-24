package {
	import flash.display.MovieClip;
	import flash.ui.Keyboard;
	import flash.text.Font;

	import UIComponent.View;

	public class Shared {
		public static var F4SEObject:Object;
		public static var F4SEPlugin:Object;

		public static var Root:MovieClip;

		public static var MainFont:Font
		public static var MainFontBold:Font

		public static var Localizations:Object;

		public static var CurrentView:View = null;

		public static const UIWidth:uint = 1280;
		public static const UIHeight:uint = 720;

		public static const Color_Primary:Number = 0xFF8339;
		public static const Color_Secondary:Number = 0x000000;
		public static const Color_Background:Number = 0x110802;
		public static const Color_DefaultBackground:Number = 0xFFFFFF;

		private static var _inputMap:Object = new Object();

		public static function ShowView(view:View) : void {
			Shared.CurrentView = view;
			HideAllView();
			Root.addChild(Shared.CurrentView);			
		}

		public static function CloseView(closeCnt:uint, closeMenuOnEmpty:Boolean = true) : void {
			if (closeCnt == 0) {
				while (Root.numChildren > 0)
					Root.removeChildAt(0);
				Shared.CurrentView = null;

				if (closeMenuOnEmpty && Shared.F4SEPlugin)
					Shared.F4SEPlugin.Close(true);
			}
			else {
				for (var ii:uint = 0; ii < closeCnt; ii++) {
					if (Root.numChildren > 1) {
						Root.removeChild(Shared.CurrentView);

						Shared.CurrentView = Root.getChildAt(Root.numChildren - 1) as View;
						Shared.CurrentView.visible = true;
					}
					else {
						if (Root.numChildren == 1) {
							Root.removeChild(Shared.CurrentView);
							Shared.CurrentView = null;
						}

						if (closeMenuOnEmpty && Shared.F4SEPlugin)
							Shared.F4SEPlugin.Close(false);
					}
				}
			}
		}

		public static function ShowMessageBox(title:String, msg:String) : void {
			var messageBox:MessageBox = new MessageBox(title, msg);
			ShowView(messageBox);
		}

		public static function ShouldHandleKey(keyCode:uint, isDown:Boolean) : Boolean {			
			if (keyCode == 0xFF)
				return false;

			if (!IsDirectionKey(keyCode))
				return isDown ? false : true;

			if (isDown) {
				var date = new Date();
				if (!_inputMap.hasOwnProperty(keyCode)) {
					_inputMap[keyCode] = date.time + 200;
					return true;
				}
				else {
					if (date.time - _inputMap[keyCode] <= 50) {
						return false;
					}
					else {
						_inputMap[keyCode] = date.time;
						return true;
					}
				}
			}
			else {
				delete _inputMap[keyCode];
				return false;
			}
		}

		private static function HideAllView() : void {
			for (var ii:uint = 0; ii < Root.numChildren; ii++)
				Root.getChildAt(ii).visible = false;
		}

		private static function IsDirectionKey(keyCode:uint) : Boolean {
			switch (keyCode) {
			case Keyboard.UP:
			case Keyboard.DOWN:
			case Keyboard.LEFT:
			case Keyboard.RIGHT:
				return true;
			}
			
			return false;
		}
	}
}
