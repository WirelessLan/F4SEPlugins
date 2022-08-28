package  {
	import F4SE.ICodeObject;
	import flash.display.MovieClip;
    import flash.display.Loader;
    import flash.net.URLRequest;
	import flash.events.Event;
	import flash.text.Font;
	import flash.events.KeyboardEvent;
	
	public class PNIOPAMenu extends MovieClip implements ICodeObject {
      	private var f4seCodeObject;
		
		public function PNIOPAMenu() {
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
			Shared.F4SE = this.f4seCodeObject;
         	Shared.F4SEPlugin = this.f4seCodeObject.plugins.PseudoNIOPA;
			
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
				ShowMainMenu(null, null, null);
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
		
		public function ShowMainMenu(selectedMenu:String, selectedGroup:String, selectedNode:String) : void {
			var mainView = new MainView(selectedNode);
			Shared.ShowView(mainView);
			
			if (selectedMenu) {
				var selectNodeGroupView = new SelectNodeGroupView(selectedNode);
				Shared.ShowView(selectNodeGroupView);
				
				if (selectedMenu == "SELECTNODE") {
					var selectNodeView = new SelectNodeView(selectedGroup, selectedNode);
					Shared.ShowView(selectNodeView);
				}
				else if (selectedMenu == "SEARCHNODE") {
					var searchNodeView = new SearchNodeView(selectedNode);
					Shared.ShowView(searchNodeView);
				}
			}
		}
	}
}