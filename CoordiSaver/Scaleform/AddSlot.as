package  {
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFieldType;
	import flash.events.MouseEvent;
	import flash.events.FocusEvent;
	
	public class AddSlot extends Sprite {		
		private var menuWidth:uint = 400;
		private var menuHeight:uint = 160;
		
		private var titleText:TextField;
		private var confirmBtn:TextField;
      	private var slotNameBox:TextField;
		
		private var showPlaceHolder:Boolean = true;

		public function AddSlot() {
        	super();
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
		
		private function slotNameBox_Clicked(evn:MouseEvent) : * {
			if (this.showPlaceHolder) {
				slotNameBox.text = "";
				this.showPlaceHolder = false;
			}
			
			if (CoordiSaverMenu.F4SEPlugin)
				CoordiSaverMenu.F4SEPlugin.AllowTextInput(true);
		}
		
		private function slotNameBox_FocusOut(evn:FocusEvent) : * {			
			if (CoordiSaverMenu.F4SEPlugin)
				CoordiSaverMenu.F4SEPlugin.AllowTextInput(false);
		}
		
		private function confirmBtn_Clicked(evn:MouseEvent) : * {
			var slotNameTxt:String = slotNameBox.text.replace(/^\s+|\s+$/g, '');
			if (slotNameTxt == "" || this.showPlaceHolder) {
				CoordiSaverMenu.Root.ShowMessageBox("슬롯 이름을 입력해주세요!");
				return;
			}
			
			if (CoordiSaverMenu.F4SEPlugin) {
				if (CoordiSaverMenu.F4SEPlugin.AddSlot(slotNameTxt)) {
					CoordiSaverMenu.Root.slotListView.GetSlotList();
					CoordiSaverMenu.Root.slotListView.DrawItems();
					CoordiSaverMenu.Root.CloseMenu(1);
				}
				else 
					CoordiSaverMenu.Root.ShowMessageBox("슬롯을 저장할 수 없습니다!");
			}
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
			this.graphics.drawRect(10, 55, menuWidth - 20, menuHeight - 115);
			
			titleText = new TextField();
			titleText.selectable = false;
			titleText.embedFonts = true;
			titleText.text = "슬롯 저장";
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
			
			slotNameBox = new TextField();
			slotNameBox.selectable = true;
			slotNameBox.embedFonts = true;
			slotNameBox.text = "슬롯 이름을 입력하세요";
			slotNameBox.x = 10;
			slotNameBox.y = 56;
			slotNameBox.width = menuWidth - 20;
			slotNameBox.height = menuHeight - 115;
			slotNameBox.type = TextFieldType.INPUT;
			slotNameBox.addEventListener(MouseEvent.CLICK, this.slotNameBox_Clicked);
			slotNameBox.addEventListener(FocusEvent.FOCUS_OUT, this.slotNameBox_FocusOut);
			//this.search.addEventListener(Event.CHANGE,this.onSearchChange);
			
			var slotNameBox_tf:TextFormat = slotNameBox.getTextFormat();
			slotNameBox_tf.color = 0xFFFFFF;
			slotNameBox_tf.font = CoordiSaverMenu.mainFont.fontName;
			slotNameBox_tf.size = 32;
			slotNameBox_tf.kerning = true;
			slotNameBox_tf.align = "center";
			
			slotNameBox.setTextFormat(slotNameBox_tf);
			
			this.addChild(slotNameBox);
			
			confirmBtn = new TextField();
			confirmBtn.y = menuHeight - 42;
			confirmBtn.x = 10;
			confirmBtn.width = menuWidth - 20;
			confirmBtn.height = 32;
			confirmBtn.text = "저장";
			confirmBtn.selectable = false;
			confirmBtn.embedFonts = true;
			confirmBtn.border = true;
			confirmBtn.borderColor = 0xFFFFFF;
			confirmBtn.addEventListener(MouseEvent.MOUSE_OUT,this.onItemMouseOut);
			confirmBtn.addEventListener(MouseEvent.MOUSE_OVER,this.onItemMouseOver);
			confirmBtn.addEventListener(MouseEvent.CLICK, this.confirmBtn_Clicked);
			
			var confirmBtn_Text:TextFormat = confirmBtn.getTextFormat();
			confirmBtn_Text.color = 0xFFFFFF;
			confirmBtn_Text.font = CoordiSaverMenu.mainFont.fontName;
			confirmBtn_Text.size = 24;
			confirmBtn_Text.kerning = true;
			confirmBtn_Text.align = "center";
			
			confirmBtn.setTextFormat(confirmBtn_Text);
			
			this.addChild(confirmBtn);
		}
	}
}