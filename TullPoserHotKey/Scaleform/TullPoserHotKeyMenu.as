package  {
	import F4SE.ICodeObject;
	import flash.display.MovieClip;
    import flash.display.Loader;
    import flash.net.URLRequest;
	import flash.events.Event;
	import flash.text.Font;
	import flash.events.KeyboardEvent;
	
	public class TullPoserHotKeyMenu extends MovieClip implements ICodeObject {
      	private var f4seCodeObject;
		
		public function TullPoserHotKeyMenu() {
        	super();
			
			Shared.Root = this;
			
			var font_urlRequest:URLRequest = new URLRequest();
			font_urlRequest.url = "fonts_en.swf";
			
			var font_loader:Loader = new Loader();
			font_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoadComplete);
			font_loader.load(font_urlRequest);
		}

		public function onF4SEObjCreated(codeObject:*) : void {
			this.f4seCodeObject = codeObject;
         	Shared.F4SEPlugin = this.f4seCodeObject.plugins.TullPoserHotKey;
			
			if (Shared.MainFont)
				Shared.F4SEPlugin.Initialize();
		}
		
		private function onLoadComplete(e:Event):void {
			var font_class:Class = e.target.applicationDomain.getDefinition("$MAIN_Font") as Class;
			Shared.MainFont = new font_class;
			Font.registerFont(font_class);
			
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.Initialize();
			else
				ShowMainMenu(null, null);
		}
		
		public function ProcessKeyEvent(ctrlName:String) : void {
			if (ctrlName == "Pipboy") {
				Shared.CloseMenu(1);
			}
			else {
				if (Shared.CurrentView)
					Shared.CurrentView.ProcessKeyEvent(ctrlName);
			}
		}
		
		public function ShowMainMenu(selectedPlugin:String, selectedPose:String) : void {
			Shared.ShowPluginListView(selectedPlugin, selectedPose);
		}
	}
}