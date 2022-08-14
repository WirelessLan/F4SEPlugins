package  {
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	public class SlotMenu extends Sprite {		
		private var menuWidth:uint = 300;
		private var menuHeight:uint = 152;
		
		private var titleText:TextField;
		
		private var slotName:String;
		
		public function SlotMenu(slotName:String) {
        	super();
			this.slotName = slotName;
			InitializeView();
		}
		
		private function onItemMouseOut(evn:MouseEvent) : * {
			var txtBox:TextField = evn.target as TextField;
			txtBox.background = false;
			txtBox.backgroundColor = 0x000000;
			
			var txtBox_tf:TextFormat = txtBox.getTextFormat();
			txtBox_tf.color = 0xFFFFFF;
			
			txtBox.setTextFormat(txtBox_tf);
		}
		
		private function onItemMouseOver(evn:MouseEvent) : * {
			var txtBox:TextField = evn.target as TextField;
			txtBox.background = true;
			txtBox.backgroundColor = 0xFFFFFF;
			
			var txtBox_tf:TextFormat = txtBox.getTextFormat();
			txtBox_tf.color = 0x000000;
			
			txtBox.setTextFormat(txtBox_tf);
		}
		
		private function loadSlotBtn_Clicked(evn:MouseEvent) : * {
			if (CoordiSaverMenu.F4SEPlugin)
				CoordiSaverMenu.F4SEPlugin.LoadSlot(slotName);
				
			CoordiSaverMenu.Root.CloseMenu(0);
		}
		
		private function deleteSlotBtn_Clicked(evn:MouseEvent) : * {
			CoordiSaverMenu.Root.ShowDeleteSlot(slotName);
		}
		
		public function InitializeView() : void {
			this.x = (CoordiSaverMenu.uiWidth - menuWidth) / 2;
			this.y = (CoordiSaverMenu.uiHeight - menuHeight) / 2;
			
			this.graphics.beginFill(0, 0.85);
			this.graphics.drawRect(0, 0, menuWidth, menuHeight);
			this.graphics.endFill();
			
			var unlineWidth:uint = 180;
			this.graphics.lineStyle(2, 0xFFFFFF);
			this.graphics.moveTo((menuWidth - unlineWidth) / 2, 45);
			this.graphics.lineTo((menuWidth - unlineWidth) / 2 + unlineWidth, 45);
			
			this.graphics.lineStyle(1, 0xFFFFFF);
			this.graphics.drawRect(10, 55, menuWidth - 20, menuHeight - 65);
			
			titleText = new TextField();
			titleText.selectable = false;
			titleText.embedFonts = true;
			titleText.text = "슬롯 옵션";
			titleText.x = 0;
			titleText.y = 10;
			titleText.width = menuWidth;
			titleText.height = 32;
			
			var titleText_tf:TextFormat = titleText.getTextFormat();
			titleText_tf.color = 0xFFFFFF;
			titleText_tf.font = CoordiSaverMenu.mainFont.fontName;
			titleText_tf.size = 24;
			titleText_tf.kerning = true;
			titleText_tf.align = "center";
			
			titleText.setTextFormat(titleText_tf);
			
			this.addChild(titleText);
			
			var loadSlotBtn:TextField = new TextField();
			loadSlotBtn.x = 15;
			loadSlotBtn.y = 60;
			loadSlotBtn.text = "슬롯 불러오기"
			loadSlotBtn.width = menuWidth - 30;
			loadSlotBtn.height = 36;
			loadSlotBtn.selectable = false;
			loadSlotBtn.embedFonts = true;
			loadSlotBtn.addEventListener(MouseEvent.MOUSE_OUT, this.onItemMouseOut);
			loadSlotBtn.addEventListener(MouseEvent.MOUSE_OVER, this.onItemMouseOver);
			loadSlotBtn.addEventListener(MouseEvent.CLICK, this.loadSlotBtn_Clicked);
			
			var loadSlotBtn_tf:TextFormat = loadSlotBtn.getTextFormat();
			loadSlotBtn_tf.color = 0xFFFFFF;
			loadSlotBtn_tf.font = CoordiSaverMenu.mainFont.fontName;
			loadSlotBtn_tf.size = 28;
			loadSlotBtn_tf.align = "center";
			loadSlotBtn_tf.kerning = true;
			
			loadSlotBtn.setTextFormat(loadSlotBtn_tf);
			
			this.addChild(loadSlotBtn);
			
			var deleteSlotBtn:TextField = new TextField();
			deleteSlotBtn.x = 15;
			deleteSlotBtn.y = 100;
			deleteSlotBtn.text = "슬롯 삭제"
			deleteSlotBtn.width = menuWidth - 30;
			deleteSlotBtn.height = 36;
			deleteSlotBtn.selectable = false;
			deleteSlotBtn.embedFonts = true;
			deleteSlotBtn.addEventListener(MouseEvent.MOUSE_OUT, this.onItemMouseOut);
			deleteSlotBtn.addEventListener(MouseEvent.MOUSE_OVER, this.onItemMouseOver);
			deleteSlotBtn.addEventListener(MouseEvent.CLICK, this.deleteSlotBtn_Clicked);
			
			var deleteSlotBtn_tf:TextFormat = deleteSlotBtn.getTextFormat();
			deleteSlotBtn_tf.color = 0xFFFFFF;
			deleteSlotBtn_tf.font = CoordiSaverMenu.mainFont.fontName;
			deleteSlotBtn_tf.size = 28;
			deleteSlotBtn_tf.align = "center";
			deleteSlotBtn_tf.kerning = true;
			
			deleteSlotBtn.setTextFormat(deleteSlotBtn_tf);
			
			this.addChild(deleteSlotBtn);
		}
	}
}