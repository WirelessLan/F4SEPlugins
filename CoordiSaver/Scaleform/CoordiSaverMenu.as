package  {
	import F4SE.ICodeObject;
    import flash.display.Loader;
    import flash.net.URLRequest;
	import flash.events.Event;
	import flash.display.MovieClip;
	import flash.text.Font;
	import fl.transitions.easing.None;
	
	public class CoordiSaverMenu extends MovieClip implements ICodeObject {
		public static var F4SEPlugin;
		public static var mainFont:Font
		
		public static var Root:CoordiSaverMenu;
		
		public static const uiWidth:uint = 1280;
		public static const uiHeight:uint = 720;
				
		public var slotListView:SlotListView = null;
		
      	private var f4seCodeObject;
		
		private var currentView = null;
		
		public function CoordiSaverMenu() {
        	super();
			
			Root = this;
			
			var font_urlRequest:URLRequest = new URLRequest();
			font_urlRequest.url = "fonts_en.swf";
			
			var font_loader:Loader = new Loader();
			font_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoadComplete);
			font_loader.load(font_urlRequest);
		}

		public function onF4SEObjCreated(codeObject:*) : void {
			this.f4seCodeObject = codeObject;
         	F4SEPlugin = this.f4seCodeObject.plugins.CoordiSaver;
			
			if (mainFont)
				F4SEPlugin.Initialize();
		}
		
		private function onLoadComplete(e:Event):void {
			var font_class:Class = e.target.applicationDomain.getDefinition("$MAIN_Font") as Class;
			mainFont = new font_class;
			Font.registerFont(font_class);
			
			if (F4SEPlugin)
				F4SEPlugin.Initialize();
			else
				ShowMenu();
		}
		
		private function hideAllChild() : void {
			for (var ii:uint = 0; ii < this.numChildren; ii++)
				this.getChildAt(ii).visible = false;
		}
		
		public function ShowMenu() : void {
			//var slotListView:SlotListView = new SlotListView();
			slotListView = new SlotListView();
			currentView = slotListView;
			hideAllChild();
			this.addChild(currentView);
		}
		
		public function ShowAddSlot() : void {				
			var addSlot:AddSlot = new AddSlot();
			currentView = addSlot;
			hideAllChild();
			this.addChild(currentView);
		}
		
		public function ShowSlotMenu(slotName:String) : void {
			var slotMenu:SlotMenu = new SlotMenu(slotName);
			currentView = slotMenu;
			hideAllChild();
			this.addChild(currentView);
		}
		
		public function ShowDeleteSlot(slotName:String) : void {
			var deleteSlot:DeleteSlot = new DeleteSlot(slotName);
			currentView = deleteSlot;
			hideAllChild();
			this.addChild(currentView);
		}
		
		public function ShowMessageBox(msg:String) : void {
			var messageBox:MessageBox = new MessageBox(msg);
			currentView = messageBox;
			hideAllChild();
			this.addChild(currentView);
		}
		
		public function CloseMenu(closeCnt:uint) : void {
			if (closeCnt == 0) {
				if (F4SEPlugin)
					F4SEPlugin.Close();
			}
			else {
				for (var ii:uint = 0; ii < closeCnt; ii++) {
					if (this.numChildren > 1) {
						if (currentView is AddSlot && F4SEPlugin)
							F4SEPlugin.AllowTextInput(false);
						this.removeChild(currentView);
						
						currentView = this.getChildAt(this.numChildren - 1);
						currentView.visible = true;
					}
					else {
						if (F4SEPlugin)
							F4SEPlugin.Close();
					}
				}
			}
		}
	}
}