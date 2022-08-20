package {
	import flash.text.Font;
	import flash.display.Sprite;
	
	public class Shared {
		public static var F4SEPlugin;
		public static var MainFont:Font
		public static var Root:TullPoserHotKeyMenu;
		
		public static var CurrentView = null;
		
		public static const UIWidth:uint = 1280;
		public static const UIHeight:uint = 720;
		
		private static function HideAllChild() : void {
			for (var ii:uint = 0; ii < Root.numChildren; ii++)
				Root.getChildAt(ii).visible = false;
		}
		
		private static function ShowView(view:Sprite) : void {
			Shared.CurrentView = view;
			HideAllChild();
			Root.addChild(Shared.CurrentView);			
		}
		
		public static function ShowMessageBox(msg:String) : void {
			var messageBox:MessageBox = new MessageBox(msg);
			ShowView(messageBox);
		}
		
		public static function ShowPluginListView(selectedPlugin:String) : void {
			var pluginListView:PluginListView = new PluginListView(selectedPlugin);
			ShowView(pluginListView);
		}
		
		public static function ShowPoseListView(plugin:String, pose:String) : void {
			var poseListView:PoseListView = new PoseListView(plugin, pose);
			ShowView(poseListView);
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
						
						Shared.CurrentView = Root.getChildAt(Root.numChildren - 1);
						Shared.CurrentView.visible = true;
					}
					else {
						Root.removeChild(Shared.CurrentView);
						Shared.CurrentView = null;
						
						if (Shared.F4SEPlugin)
							Shared.F4SEPlugin.Close(false);
					}
				}
			}
		}
	}
}
