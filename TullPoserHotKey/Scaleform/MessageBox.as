package  {
	import flash.text.TextField;
	import flash.text.TextFormat;
	import UIComponent.View;
	import UIComponent.Button;
	import UIComponent.UIEvent;
	
	public class MessageBox extends View {		
		private var menuWidth:uint = 400;
		private var menuHeight:uint = 160;
		
		private var message:String;
		
		private var titleText:TextField;
      	private var messageBox:TextField;
		private var confirmBtn:Button;

		public function MessageBox(msg:String) {
			this.message = msg;
        	super(menuWidth, menuHeight, "메시지");
		}
		
		private function confirmBtn_Activated(evn:UIEvent) : * {
			Shared.CloseMenu(1);
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			messageBox = new TextField();
			messageBox.selectable = false;
			messageBox.embedFonts = true;
			messageBox.text = message;
			messageBox.x = 10;
			messageBox.y = 56;
			messageBox.width = menuWidth - 20;
			messageBox.height = menuHeight - 115;
			
			var messageBox_tf:TextFormat = messageBox.getTextFormat();
			messageBox_tf.color = 0xFFFFFF;
			messageBox_tf.font = Shared.MainFont.fontName;
			messageBox_tf.size = 32;
			messageBox_tf.kerning = true;
			messageBox_tf.align = "center";
			
			messageBox.setTextFormat(messageBox_tf);
			
			this.AddComponent(messageBox);
			
			confirmBtn = new Button(menuWidth - 20, 32);
			confirmBtn.text = "OK";
			confirmBtn.y = menuHeight - 42;
			confirmBtn.x = 10;
			confirmBtn.addEventListener(UIEvent.ACTIVATE, confirmBtn_Activated);
			
			this.AddComponent(confirmBtn);
		}
	}
}