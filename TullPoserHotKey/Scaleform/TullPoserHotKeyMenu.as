package  {
	import F4SE.ICodeObject;
	import flash.display.MovieClip;
    import flash.display.Loader;
    import flash.net.URLRequest;
	import flash.events.Event;
	import flash.text.Font;
	
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
				ShowMainMenu(null);
		}
		
		public function ShowMainMenu(selectedPlugin:String) : void {
			Shared.ShowPluginListView(selectedPlugin);
		}
		
		public function ShowPoseMenu(plugin:String, pose:String) : void {
			Shared.ShowPoseListView(plugin, pose);
		}
		
		public function CloseMenu() : void {
			Shared.CloseMenu(1);
		}
	}
}