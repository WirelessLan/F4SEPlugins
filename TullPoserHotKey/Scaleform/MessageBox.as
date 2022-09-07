package  {
	import UIComponent.View;
	import UIComponent.Button;
	import UIComponent.Label;
	import UIComponent.UIEvent;
	
	public class MessageBox extends View {		
		private var menuWidth:uint = 400;
		private var menuHeight:uint = 160;
		
		private var title:String;
		private var message:String;
		
		private var messageLbl:Label;
		private var confirmBtn:Button;

		public function MessageBox(title:String, msg:String) {
			this.title = title;
			this.message = msg;
        	super(menuWidth, menuHeight, this.title);
		}
		
		private function confirmBtn_Activated(evn:UIEvent) : * {
			Shared.CloseMenu(1);
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			messageLbl = new Label(menuWidth - 20, menuHeight - 115, message, 24);
			messageLbl.x = 10;
			messageLbl.y = 65;
			messageLbl.width = menuWidth - 20;
			messageLbl.height = menuHeight - 115;			
			this.AddComponent(messageLbl);
			
			confirmBtn = new Button(menuWidth - 20, 32);
			confirmBtn.text = "OK";
			confirmBtn.y = menuHeight - 42;
			confirmBtn.x = 10;
			confirmBtn.addEventListener(UIEvent.ACTIVATE, confirmBtn_Activated);
			this.AddComponent(confirmBtn);
		}
	}
}