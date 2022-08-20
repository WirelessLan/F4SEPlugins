package  {
	import flash.display.Sprite;
	import flash.geom.Rectangle;
	import flash.text.Font;
	import flash.text.TextFormat;
	import flash.text.TextField;
	import flash.events.Event;
	import flash.events.MouseEvent;
	
	public class PluginListView extends Sprite {		
		private var menuWidth:uint = 300;
		private var menuHeight:uint = 620;
		
		private var titleText:TextField;
		private var itemsContainer:Sprite;
		private var itemsContainerBtnHeight:uint = 32;
		private var scrollbar:Scrollbar;
				
		private var items:Array;

		public function PluginListView(selectedPlugin:String) {
        	super();
			GetPluginList();
			InitializeView();
			
			var targetItem:TextField
			for (var ii = 0; ii < items.length; ii++) {
				var itemTb:TextField = itemsContainer.getChildAt(ii) as TextField;
				if (selectedPlugin && selectedPlugin == itemTb.text) {
					targetItem = itemTb;
					break;
				}
			}
			
			if (selectedPlugin && targetItem)
				SetScroll(targetItem.y);
		}
		
		public function GetPluginList() {
			if (Shared.F4SEPlugin)
				items = Shared.F4SEPlugin.GetPluginList();
			else
				items = new Array("TestPlugin1.esp", "TestPlugin2.esp");
		}
      
		private function get maxScrollOffset() : Number	{
			return items.length * itemsContainerBtnHeight > itemsContainer.scrollRect.height ? 
				Number(items.length * itemsContainerBtnHeight - itemsContainer.scrollRect.height) : Number(0);
		}
		
		public function SetScroll(newY:Number) : void {
			if (this.maxScrollOffset == 0)
				return;
				
			if (newY > this.maxScrollOffset)
				newY = this.maxScrollOffset;
			
			if (itemsContainer.scrollRect.y == newY)
				return;
			
			var sRect:Rectangle = itemsContainer.scrollRect;
			sRect.y = newY;
			itemsContainer.scrollRect = sRect;
			var scrollPer:Number = 100 * sRect.y / this.maxScrollOffset;
			scrollbar.setScrollPercent(scrollPer);
		}
		
		private function onMouseWheel(evn:MouseEvent) : * {
			var wheel_d:int = evn.delta;
			var currY:Number = itemsContainer.scrollRect.y;
			if (wheel_d <= 0 && currY < this.maxScrollOffset) {
				currY -= wheel_d * itemsContainerBtnHeight;
				if (currY > this.maxScrollOffset)
					currY = this.maxScrollOffset;
			}
			else if (wheel_d > 0 && currY > 0) {
				currY -= wheel_d * itemsContainerBtnHeight;
				if (currY < 0)
					currY = 0;
			}
			
			SetScroll(currY);
		}
		
		private function HighlightItem(item:TextField, onOff:Boolean) : void {
			var item_tf:TextFormat = item.getTextFormat();
			
			if (onOff) {
				item.background = true;
				item.backgroundColor = 0xFFFFFF;
				
				item_tf.color = 0x000000;
			}
			else {
				item.background = false;
				item.backgroundColor = 0x000000;
				
				item_tf.color = 0xFFFFFF;
			}
			
			item.setTextFormat(item_tf);
		}
		
		private function onItemMouseOver(evn:MouseEvent) : * {
			var txtBox:TextField = evn.target as TextField;
			HighlightItem(txtBox, true);
		}
		
		private function onItemMouseOut(evn:MouseEvent) : * {
			var txtBox:TextField = evn.target as TextField;
			HighlightItem(txtBox, false);
		}
		
		private function onItemClick(evn:MouseEvent) : * {
			var txtBox:TextField = evn.target as TextField;
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SelectPlugin(txtBox.text, txtBox.y);
			Shared.ShowPoseListView(txtBox.text, null);
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
			
			this.graphics.lineStyle(1, 0xFFFFFF);
			this.graphics.drawRect(10, 55, menuWidth - 20, menuHeight - 65);
			
			titleText = new TextField();
			titleText.selectable = false;
			titleText.embedFonts = true;
			titleText.text = "플러그인 선택";
			titleText.x = 0;
			titleText.y = 10;
			titleText.width = menuWidth;
			
			var titleText_tf:TextFormat = titleText.getTextFormat();
			titleText_tf.color = 0xFFFFFF;
			titleText_tf.font = Shared.MainFont.fontName;
			titleText_tf.size = 20;
			titleText_tf.kerning = true;
			titleText_tf.align = "center";
			
			titleText.setTextFormat(titleText_tf);
			
			this.addChild(titleText);
			
			itemsContainer = new Sprite();
			itemsContainer.cacheAsBitmap = true;
			itemsContainer.scrollRect = new Rectangle(0, 0, menuWidth - 30, menuHeight - 75);
			itemsContainer.x = 15;
			itemsContainer.y = 60;
			itemsContainer.addEventListener(MouseEvent.MOUSE_WHEEL, onMouseWheel);
			
			this.addChild(itemsContainer);

			DrawItems();
		}
		
		public function DrawItems() : void {
			while (itemsContainer.numChildren > 0)
				itemsContainer.removeChildAt(0);
				
			if (scrollbar)
				this.removeChild(scrollbar);
				
			scrollbar = new Scrollbar(itemsContainer.scrollRect.height, items.length * itemsContainerBtnHeight, items.length);
			scrollbar.x = itemsContainer.x + itemsContainer.scrollRect.width;
			scrollbar.y = itemsContainer.y;
			
			this.addChild(scrollbar);
			
			var itemIdx:uint = 0;
			var nextHeight:uint = 1;
			while (itemIdx < items.length) {
				var textBox:TextField = new TextField();
				
				textBox.y = nextHeight;
				textBox.text = items[itemIdx];
				textBox.width = menuWidth - 30;
				textBox.height = itemsContainerBtnHeight;
				textBox.selectable = false;
				textBox.embedFonts = true;
				textBox.addEventListener(MouseEvent.MOUSE_OUT, onItemMouseOut);
				textBox.addEventListener(MouseEvent.MOUSE_OVER, onItemMouseOver);
				textBox.addEventListener(MouseEvent.CLICK, onItemClick);
				
				var txtFormat:TextFormat = textBox.getTextFormat();
				txtFormat.color = 0xFFFFFF;
				txtFormat.font = Shared.MainFont.fontName;
				txtFormat.size = itemsContainerBtnHeight - 8;
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