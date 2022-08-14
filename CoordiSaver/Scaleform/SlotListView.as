package  {
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.display.Sprite;
	import flash.geom.Rectangle;
	import flash.text.Font;
	import flash.text.TextFormat;
	import flash.text.TextField;
	
	public class SlotListView extends Sprite {		
		private var menuWidth:uint = 300;
		private var menuHeight:uint = 620;
		
		private var titleText:TextField;
		private var itemsContainer:Sprite;
		private var itemsContainerBtnHeight:uint = 36;
		private var scrollbar:Scrollbar;
		private var newItemBtn:TextField;
				
		public var menuItems:Array;

		public function SlotListView() {
        	super();
			GetSlotList();
			InitializeView();
		}
		
		public function GetSlotList() {
			if (CoordiSaverMenu.F4SEPlugin)
				menuItems = CoordiSaverMenu.F4SEPlugin.GetSlotList();
			else
				menuItems = new Array();
		}
      
		private function get maxScrollOffset() : Number	{
			return menuItems.length * itemsContainerBtnHeight > itemsContainer.scrollRect.height ? 
				Number(menuItems.length * itemsContainerBtnHeight - itemsContainer.scrollRect.height) : Number(0);
		}
		
		private function onMouseWheel(evn:MouseEvent) : * {
			var wheel_d:int = evn.delta;
			var sRect:Rectangle = itemsContainer.scrollRect;
			var currY:* = sRect.y;
			if (wheel_d <= 0 && currY < this.maxScrollOffset) {
				sRect.y -= wheel_d * itemsContainerBtnHeight;
				if(sRect.y > this.maxScrollOffset)
					sRect.y = this.maxScrollOffset;
			}
			else if (wheel_d > 0 && currY > 0) {
				sRect.y -= wheel_d * itemsContainerBtnHeight;
				if(sRect.y < 0)
					sRect.y = 0;
			}
			itemsContainer.scrollRect = sRect;
			var scrollPer:Number = 100 * sRect.y / this.maxScrollOffset;
			scrollbar.setScrollPercent(scrollPer);
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
		
		private function onItemClick(evn:MouseEvent) : * {
			var item:TextField = evn.target as TextField;
			CoordiSaverMenu.Root.ShowSlotMenu(item.text);
		}
		
		private function newItemBtn_Clicked(evn:MouseEvent) : * {
			CoordiSaverMenu.Root.ShowAddSlot();
		}
		
		public function InitializeView() : void {
			this.x = (CoordiSaverMenu.uiWidth - menuWidth) / 2;
			this.y = (CoordiSaverMenu.uiHeight - menuHeight) / 2;
			this.cacheAsBitmap = true;
			
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
			titleText.text = "Coordi Saver";
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
			
			itemsContainer = new Sprite();
			itemsContainer.scrollRect = new Rectangle(0, 0, menuWidth - 30, menuHeight - 125);
			itemsContainer.x = 15;
			itemsContainer.y = 61;
			itemsContainer.addEventListener(MouseEvent.MOUSE_WHEEL, this.onMouseWheel);
			
			this.addChild(itemsContainer);
			
			newItemBtn = new TextField();
			newItemBtn.y = menuHeight - 50;
			newItemBtn.x = 10;
			newItemBtn.width = menuWidth - 20;
			newItemBtn.height = 40;
			newItemBtn.text = "+";
			newItemBtn.selectable = false;
			newItemBtn.embedFonts = true;
			newItemBtn.border = true;
			newItemBtn.borderColor = 0xFFFFFF;
			newItemBtn.addEventListener(MouseEvent.MOUSE_OUT,this.onItemMouseOut);
			newItemBtn.addEventListener(MouseEvent.MOUSE_OVER,this.onItemMouseOver);
			newItemBtn.addEventListener(MouseEvent.CLICK, this.newItemBtn_Clicked);
			
			var newItemBtn_tf:TextFormat = newItemBtn.getTextFormat();
			newItemBtn_tf.color = 0xFFFFFF;
			newItemBtn_tf.font = CoordiSaverMenu.mainFont.fontName;
			newItemBtn_tf.size = 30;
			newItemBtn_tf.kerning = true;
			newItemBtn_tf.align = "center";
			
			newItemBtn.setTextFormat(newItemBtn_tf);
			
			this.addChild(newItemBtn);
			
			DrawItems();
		}
		
		public function DrawItems() : void {
			while (itemsContainer.numChildren > 0)
				itemsContainer.removeChildAt(0);
				
			if (scrollbar)
				this.removeChild(scrollbar);
				
			scrollbar = new Scrollbar(itemsContainer.scrollRect.height, menuItems.length * itemsContainerBtnHeight, menuItems.length);
			scrollbar.x = menuWidth - 15;
			scrollbar.y = 60;
			
			this.addChild(scrollbar);
			
			itemsContainer.scrollRect = new Rectangle(0, 0, menuWidth - 30, menuHeight - 125);
			
			var itemIdx:uint = 0;
			var nextHeight:uint = 1;
			while (itemIdx < menuItems.length) {
				var textBox:TextField = new TextField();
				
				textBox.y = nextHeight;
				textBox.text = menuItems[itemIdx];
				textBox.width = menuWidth - 30;
				textBox.height = itemsContainerBtnHeight;
				textBox.selectable = false;
				textBox.embedFonts = true;
				textBox.addEventListener(MouseEvent.MOUSE_OUT, this.onItemMouseOut);
				textBox.addEventListener(MouseEvent.MOUSE_OVER, this.onItemMouseOver);
				textBox.addEventListener(MouseEvent.CLICK, this.onItemClick);
				
				var txtFormat:TextFormat = textBox.getTextFormat();
				txtFormat.color = 0xFFFFFF;
				txtFormat.font = CoordiSaverMenu.mainFont.fontName;
				txtFormat.size = 30;
				txtFormat.align = "left";
				txtFormat.kerning = true;
				
				textBox.setTextFormat(txtFormat);
				itemsContainer.addChild(textBox);
				
				nextHeight += itemsContainerBtnHeight;
				itemIdx++;
			}
		}
	}
}