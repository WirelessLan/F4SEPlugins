package {
	import flash.text.Font;
	import flash.display.MovieClip;
	import UIComponent.View;
	
	public class Shared {
		public static var F4SEPlugin;
		public static var MainFont:Font
		public static var Root:MovieClip;
		
		public static var CurrentView:View = null;
		
		public static const UIWidth:uint = 1280;
		public static const UIHeight:uint = 720;
		
		public static const Color_Primary:Number = 0xFFFFFF;
		public static const Color_Secondary:Number = 0x000000;
		public static const Color_Background:Number = 0x282828;
		public static const Color_DefaultBackground:Number = 0xFFFFFF;
		
		private static function HideAllChild() : void {
			for (var ii:uint = 0; ii < Root.numChildren; ii++)
				Root.getChildAt(ii).visible = false;
		}
		
		public static function ShowView(view:View) : void {
			Shared.CurrentView = view;
			HideAllChild();
			Root.addChild(Shared.CurrentView);			
		}
		
		public static function ShowMessageBox(title:String, msg:String) : void {
			var messageBox:MessageBox = new MessageBox(title, msg);
			ShowView(messageBox);
		}
		
		public static function CloseMenu(closeCnt:uint) : void {
			if (closeCnt == 0) {
				while (Root.numChildren > 0)
					Root.removeChildAt(0);
				Shared.CurrentView = null;
					
				if (Shared.F4SEPlugin)
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
						if (Shared.F4SEPlugin)
							Shared.F4SEPlugin.Close(false);
					}
				}
			}
		}
	}
}
