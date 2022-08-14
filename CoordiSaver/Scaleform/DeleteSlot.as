package  {
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	public class DeleteSlot extends Sprite {		
		private var menuWidth:uint = 500;
		private var menuHeight:uint = 205;
		
		private var titleText:TextField;
      	private var messageBox:TextField;
		
		private var slotName:String;
		
		public function DeleteSlot(slotName:String) {
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
		
		private function deleteSlotBtn_Clicked(evn:MouseEvent) : * {
			if (CoordiSaverMenu.F4SEPlugin) {
				if (CoordiSaverMenu.F4SEPlugin.DeleteSlot(slotName)) {
					CoordiSaverMenu.Root.slotListView.GetSlotList();
					CoordiSaverMenu.Root.slotListView.DrawItems();
					CoordiSaverMenu.Root.CloseMenu(2);
				}
				else
					CoordiSaverMenu.Root.ShowMessageBox("슬롯을 삭제할 수 없습니다!");
			}
		}
		
		private function cancelBtn_Clicked(evn:MouseEvent) : * {
			CoordiSaverMenu.Root.CloseMenu(1);
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
			
			titleText = new TextField();
			titleText.selectable = false;
			titleText.embedFonts = true;
			titleText.text = "슬롯 삭제";
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
			
			messageBox = new TextField();
			messageBox.selectable = false;
			messageBox.embedFonts = true;
			messageBox.text = "정말 이 슬롯을 삭제하시겠습니까?";
			messageBox.x = 10;
			messageBox.y = 56;
			messageBox.width = menuWidth - 20;
			messageBox.height = menuHeight - 115;
			
			var messageBox_tf:TextFormat = messageBox.getTextFormat();
			messageBox_tf.color = 0xFFFFFF;
			messageBox_tf.font = CoordiSaverMenu.mainFont.fontName;
			messageBox_tf.size = 32;
			messageBox_tf.kerning = true;
			messageBox_tf.align = "center";
			
			messageBox.setTextFormat(messageBox_tf);
			
			this.addChild(messageBox);
			
			var deleteSlotBtn:TextField = new TextField();
			deleteSlotBtn.x = 15;
			deleteSlotBtn.y = 110;
			deleteSlotBtn.text = "삭제"
			deleteSlotBtn.width = menuWidth - 30;
			deleteSlotBtn.height = 36;
			deleteSlotBtn.selectable = false;
			deleteSlotBtn.embedFonts = true;
			deleteSlotBtn.border = true;
			deleteSlotBtn.borderColor = 0xFFFFFF;
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
			
			var cancelBtn:TextField = new TextField();
			cancelBtn.x = 15;
			cancelBtn.y = 150;
			cancelBtn.text = "취소"
			cancelBtn.width = menuWidth - 30;
			cancelBtn.height = 36;
			cancelBtn.selectable = false;
			cancelBtn.embedFonts = true;
			cancelBtn.border = true;
			cancelBtn.borderColor = 0xFFFFFF;
			cancelBtn.addEventListener(MouseEvent.MOUSE_OUT, this.onItemMouseOut);
			cancelBtn.addEventListener(MouseEvent.MOUSE_OVER, this.onItemMouseOver);
			cancelBtn.addEventListener(MouseEvent.CLICK, this.cancelBtn_Clicked);
			
			var cancelBtn_tf:TextFormat = cancelBtn.getTextFormat();
			cancelBtn_tf.color = 0xFFFFFF;
			cancelBtn_tf.font = CoordiSaverMenu.mainFont.fontName;
			cancelBtn_tf.size = 28;
			cancelBtn_tf.align = "center";
			cancelBtn_tf.kerning = true;
			
			cancelBtn.setTextFormat(cancelBtn_tf);
			
			this.addChild(cancelBtn);
		}
	}
}