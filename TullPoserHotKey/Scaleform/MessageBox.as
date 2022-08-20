package  {
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFieldType;
	import flash.events.MouseEvent;
	
	public class MessageBox extends Sprite {		
		private var menuWidth:uint = 400;
		private var menuHeight:uint = 160;
		
		private var message:String;
		
		private var titleText:TextField;
		private var confirmBtn:TextField;
      	private var messageBox:TextField;

		public function MessageBox(msg:String) {
        	super();
			this.message = msg;
			InitializeView();
		}
		
		private function onItemMouseOver(evn:MouseEvent) : * {
			var txtBox:TextField = evn.target as TextField;
			txtBox.background = true;
			txtBox.backgroundColor = 0xFFFFFF;
			
			var txtBox_tf:TextFormat = txtBox.getTextFormat();
			txtBox_tf.color = 0x000000;
			
			txtBox.setTextFormat(txtBox_tf);
		}
		
		private function onItemMouseOut(evn:MouseEvent) : * {
			var txtBox:TextField = evn.target as TextField;
			txtBox.background = false;
			txtBox.backgroundColor = 0x000000;
			
			var txtBox_tf:TextFormat = txtBox.getTextFormat();
			txtBox_tf.color = 0xFFFFFF;
			
			txtBox.setTextFormat(txtBox_tf);
		}
		
		private function confirmBtn_Clicked(evn:MouseEvent) : * {
			Shared.CloseMenu(1);
		}
		
		public function InitializeView() : void {
			this.x = (Shared.UIWidth - menuWidth) / 2;
			this.y = (Shared.UIHeight - menuHeight) / 2;
			
			this.graphics.beginFill(0, 0.85);
			this.graphics.drawRect(0, 0, menuWidth, menuHeight);
			this.graphics.endFill();
			
			var unlineWidth:uint = 180;
			this.graphics.lineStyle(2, 0xFFFFFF);
			this.graphics.moveTo((menuWidth - unlineWidth) / 2, 45);
			this.graphics.lineTo((menuWidth - unlineWidth) / 2 + unlineWidth, 45);
			
			titleText = new TextField();
			titleText.selectable = false;
			titleText.embedFonts = true;
			titleText.text = "Message";
			titleText.x = 0;
			titleText.y = 10;
			titleText.width = menuWidth;
			titleText.height = 32;
			
			var titleText_tf:TextFormat = titleText.getTextFormat();
			titleText_tf.color = 0xFFFFFF;
			titleText_tf.font = Shared.MainFont.fontName;
			titleText_tf.size = 24;
			titleText_tf.kerning = true;
			titleText_tf.align = "center";
			
			titleText.setTextFormat(titleText_tf);
			
			this.addChild(titleText);
			
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
			
			this.addChild(messageBox);
			
			confirmBtn = new TextField();
			confirmBtn.y = menuHeight - 42;
			confirmBtn.x = 10;
			confirmBtn.width = menuWidth - 20;
			confirmBtn.height = 32;
			confirmBtn.text = "OK";
			confirmBtn.selectable = false;
			confirmBtn.embedFonts = true;
			confirmBtn.border = true;
			confirmBtn.borderColor = 0xFFFFFF;
			confirmBtn.addEventListener(MouseEvent.MOUSE_OUT,this.onItemMouseOut);
			confirmBtn.addEventListener(MouseEvent.MOUSE_OVER,this.onItemMouseOver);
			confirmBtn.addEventListener(MouseEvent.CLICK,this.confirmBtn_Clicked);
			
			var confirmBtn_Text:TextFormat = confirmBtn.getTextFormat();
			confirmBtn_Text.color = 0xFFFFFF;
			confirmBtn_Text.font = Shared.MainFont.fontName;
			confirmBtn_Text.size = 24;
			confirmBtn_Text.kerning = true;
			confirmBtn_Text.align = "center";
			
			confirmBtn.setTextFormat(confirmBtn_Text);
			
			this.addChild(confirmBtn);
		}
	}
}