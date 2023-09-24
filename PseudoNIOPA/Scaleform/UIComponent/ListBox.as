﻿package UIComponent {
	import flash.display.Sprite;
	import flash.geom.Rectangle;
	import flash.geom.Point;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.filters.DropShadowFilter;
	import flash.ui.Keyboard;
	import flash.events.MouseEvent;

	public class ListBox extends Sprite implements IComponent {
		private var _width:Number;
		private var _height:Number;

		private var _highlightColor:Number = 0x42E0D1;

		private var _itemArr:Array = null;
		private var _itemHeight:uint = 28;
		private var _focusedItem:TextField = null;
		private var _highlightedItemIndex:int = -1;

		private var itemsContainer:Sprite;
		private var scrollbar:Scrollbar;

		public function ListBox(width:Number, height:Number) {
			super();

			this._width = width;
			this._height = height;

			this._itemArr = new Array();

			initializeComponent();
		}

		public function Activate() : void {
			if (!this._focusedItem)
				return;

			this.dispatchEvent(new UIEvent(UIEvent.ACTIVATE));
		}

		public function CanChangeFocus(dir:int) : Boolean {
			if (this._itemArr.length == 0)
				return true;

			if (!this._focusedItem)
				return false;

			if (dir == FocusDirection.Previous && this.itemsContainer.getChildIndex(this._focusedItem) == 0)
				return true;
			else if (dir == FocusDirection.Next && this.itemsContainer.getChildIndex(this._focusedItem) == this.itemsContainer.numChildren - 1)
				return true;
			return false;
		}

		public function IsFocusable() : Boolean {
			if (this.itemsContainer.numChildren == 0)
				return false;
			return true;
		}

		public function SetFocus(onOff:Boolean) : void{
			if (!onOff && this._focusedItem) {
				focusItem(this._focusedItem, onOff);
				this._focusedItem = null;
			}
		}

		public function ProcessKeyEvent(keyCode:uint) : void {
			if (this.itemsContainer.numChildren == 0)
				return;

			var targetIdx:uint;
			var focusedIndex:int;

			if (keyCode == Keyboard.UP) {
				if (!this._focusedItem) {
					targetIdx = this.itemsContainer.numChildren - 1;
					focusItem(this.itemsContainer.getChildAt(targetIdx) as TextField, true);
				}
				else {
					focusedIndex = this.itemsContainer.getChildIndex(this._focusedItem) - 1;
					if (focusedIndex < 0)
						focusedIndex = 0;
					focusItem(this._focusedItem, false);
					focusItem(this.itemsContainer.getChildAt(focusedIndex) as TextField, true);
				}
			}
			else if (keyCode == Keyboard.DOWN) {
				if (!this._focusedItem) {
					targetIdx = 0;
					focusItem(this.itemsContainer.getChildAt(targetIdx) as TextField, true);
				}
				else {
					focusedIndex = this.itemsContainer.getChildIndex(this._focusedItem) + 1;
					if (focusedIndex >= this.itemsContainer.numChildren)
						focusedIndex = this.itemsContainer.numChildren - 1;
					focusItem(this._focusedItem, false);
					focusItem(this.itemsContainer.getChildAt(focusedIndex) as TextField, true);
				}
			}
		}

		public function SetItems(items:Array) : void {
			if (!items)
				return;

			this._itemArr = items;
			drawItems();
		}

		public function AddItem(newItem:String) : void {
			this._itemArr.push(newItem);
			drawItems();
		}

		public function RemoveItemAt(index:uint) : void {
			this._itemArr.splice(index, 1);
			drawItems();
		}

		public function Highlight(item:String) : void {
			var itemIndex:uint = 0;
			for (; itemIndex < this._itemArr.length; itemIndex++) {
				if (item.toLowerCase() == (this._itemArr[itemIndex] as String).toLowerCase())
					break;
			}

			if (itemIndex == this._itemArr.length)
				return;

			if (this._focusedItem)
				focusItem(this._focusedItem, false);

			var targetItemBtn:TextField = this.itemsContainer.getChildAt(itemIndex) as TextField;
			if (targetItemBtn) {
				if (this._highlightedItemIndex >= 0) {
					var highlightedItem:TextField = this.itemsContainer.getChildAt(this._highlightedItemIndex) as TextField;
					highlightItem(highlightedItem, false);
				}
				this._highlightedItemIndex = itemIndex;
				highlightItem(targetItemBtn, true);
				focusItem(targetItemBtn, true);
			}
		}

		public function get SelectedItem() : String	{
			if (!this._focusedItem)
				return null;

			return this._focusedItem.text;
		}

		private function initializeComponent() {
			this.graphics.lineStyle(1, Shared.Color_Primary);
			this.graphics.drawRect(0, 0, this._width, this._height);

			this.itemsContainer = new Sprite();
			this.itemsContainer.cacheAsBitmap = true;
			this.itemsContainer.x = 5;
			this.itemsContainer.y = 5;
			this.itemsContainer.scrollRect = new Rectangle(0, 0, this._width - 5, this._height - 10);
			this.itemsContainer.addEventListener(MouseEvent.MOUSE_WHEEL, onMouseWheel);

			this.addChild(this.itemsContainer);

			this.scrollbar = new Scrollbar(15, this.itemsContainer.scrollRect.height);
			this.scrollbar.x = this.itemsContainer.x + this.itemsContainer.scrollRect.width - this.scrollbar.width ;
			this.scrollbar.y = this.itemsContainer.y;
			this.scrollbar.addEventListener(MouseEvent.MOUSE_DOWN, onScrollDown);
			this.scrollbar.addEventListener(MouseEvent.CLICK, onScrollClick);
			this.scrollbar.addEventListener(MouseEvent.MOUSE_WHEEL, onMouseWheel);

			this.addChild(this.scrollbar);

			drawItems();
		}

		private function drawItems() : void {
			while (this.itemsContainer.numChildren > 0)
				this.itemsContainer.removeChildAt(0);

			var defaultItemTextFormat:TextFormat = new TextFormat(Shared.MainFont.fontName, this._itemHeight - 8, Shared.Color_Primary);
			defaultItemTextFormat.kerning = true;
			defaultItemTextFormat.align = "left";

			var totalHeight = this._itemHeight * this._itemArr.length;

			var itemIdx:uint = 0;
			var nextHeight:uint = 1;
			while (itemIdx < this._itemArr.length) {
				var newItem:TextField = new TextField();
				newItem.y = nextHeight;
				newItem.width = this._width - (this.itemsContainer.scrollRect.height > totalHeight ? 10 : 20);
				newItem.height = this._itemHeight;
				newItem.selectable = false;
				newItem.embedFonts = true;
				newItem.defaultTextFormat = defaultItemTextFormat;
				newItem.filters = [new DropShadowFilter(0.5, 45, Shared.Color_Primary, 0.6, 0, 0, 1.0, 0.3, false, false, false)];
				newItem.text = this._itemArr[itemIdx];
				newItem.addEventListener(MouseEvent.MOUSE_OVER, onItemMouseOver);
				newItem.addEventListener(MouseEvent.CLICK, onItemClick);

				if (itemIdx == _highlightedItemIndex)
					focusItem(newItem, true);

				this.itemsContainer.addChild(newItem);

				nextHeight += newItem.height;
				itemIdx++;
			}

			this.scrollbar.SetThumbHeight(this.itemsContainer.scrollRect.height, totalHeight);
		}

		private function get maxScrollOffset() : Number	{
			return this._itemArr.length * this._itemHeight > this.itemsContainer.scrollRect.height ? 
				Number(this._itemArr.length * this._itemHeight - this.itemsContainer.scrollRect.height) : Number(0);
		}

		private function setScroll(newY:Number, moveScrollbar:Boolean) : void {
			if (this.maxScrollOffset == 0)
				return;

			if (newY < 0)
				newY = 0;

			if (newY > this.maxScrollOffset)
				newY = this.maxScrollOffset;

			if (itemsContainer.scrollRect.y == newY)
				return;

			var sRect:Rectangle = itemsContainer.scrollRect;
			sRect.y = newY;
			itemsContainer.scrollRect = sRect;

			if (moveScrollbar) {
				var scrollPer:Number = 100 * sRect.y / this.maxScrollOffset;
				this.scrollbar.SetThumbPosition(scrollPer);
			}
		}

		private function calculateScrollPosition(item:TextField) : Number {
			if (!item)
				return this.itemsContainer.scrollRect.y;

			if (item.y < this.itemsContainer.scrollRect.y)
				return item.y;
			else if (item.y + this._itemHeight > this.itemsContainer.scrollRect.y + this.itemsContainer.scrollRect.height)
				return item.y - this.itemsContainer.scrollRect.height + this._itemHeight;

			return this.itemsContainer.scrollRect.y;
		}

		private function focusItem(item:TextField, onOff:Boolean) : void {
			if (!item)
				return;

			var itemIdx:int = this.itemsContainer.getChildIndex(item);
			var focused:Boolean = false;

			if (itemIdx >= 0 && itemIdx == this._highlightedItemIndex)
				focused = true;

			var item_tf:TextFormat = item.getTextFormat();

			if (onOff) {
				item.background = true;
				item.backgroundColor = Shared.Color_Primary;

				if (!focused)
					item_tf.color = Shared.Color_Secondary;

				this._focusedItem = item;

				setScroll(calculateScrollPosition(this._focusedItem), true);
			}
			else {
				item.background = false;
				item.backgroundColor = Shared.Color_Secondary;

				if (!focused)
					item_tf.color = Shared.Color_Primary;
			}

			item.setTextFormat(item_tf);
		}

		private function highlightItem(item:TextField, onOff:Boolean) : void {
			if (!item)
				return;

			var item_tf:TextFormat = item.getTextFormat();
			if (onOff)
				item_tf.color = this._highlightColor;
			else
				item_tf.color = Shared.Color_Primary;

			item.setTextFormat(item_tf);
		}

		private function onMouseWheel(evn:MouseEvent) : * {
			var wheel_d:int = evn.delta;
			var currY:Number = itemsContainer.scrollRect.y;

			if (wheel_d <= 0 && currY < this.maxScrollOffset) {
				currY -= wheel_d * this._itemHeight;
				if (currY > this.maxScrollOffset)
					currY = this.maxScrollOffset;
			}
			else if (wheel_d > 0 && currY > 0) {
				currY -= wheel_d * this._itemHeight;
				if (currY < 0)
					currY = 0;
			}

			setScroll(currY, true);
		}

		private function onItemMouseOver(evn:MouseEvent) : * {
			evn.stopPropagation();
			var txtBox:TextField = evn.target as TextField;
			if (this._focusedItem && this._focusedItem != txtBox)
				focusItem(this._focusedItem, false);
			focusItem(txtBox, true);
		}

		private function onItemClick(evn:MouseEvent) : * {
			this.Activate();
		}

		private function onScrollDown(evn:MouseEvent) {
			if (!this.scrollbar.IsHighlighted())
				return;

			stage.addEventListener(MouseEvent.MOUSE_MOVE, stage_onMouseMove);
			stage.addEventListener(MouseEvent.MOUSE_UP, stage_onMouseUp);
		}

		private function stage_onMouseMove(evn:MouseEvent) {
			if (!this.scrollbar.IsDragging())
				return;

			var areaHeight = this.itemsContainer.scrollRect.height;
			var totalHeight = this._itemHeight * this._itemArr.length;

			if (areaHeight == 0 || totalHeight == 0)
				return;

			var newY = this.globalToLocal(new Point(0, stage.mouseY)).y;

			var percent:Number = newY / areaHeight;
			var scrollPos:Number = percent * totalHeight;
			var thumbHeight:Number = areaHeight / totalHeight * areaHeight;
			var thumbPercent:Number = thumbHeight / areaHeight;
			scrollPos -= thumbPercent * totalHeight / 2;
			setScroll(scrollPos, true);
		}

		private function stage_onMouseUp(evn:MouseEvent) {
			stage.removeEventListener(MouseEvent.MOUSE_MOVE, stage_onMouseMove);
			stage.removeEventListener(MouseEvent.MOUSE_UP, stage_onMouseUp);
		}

		private function onScrollClick(evn:MouseEvent) {
			if (this.scrollbar.IsHighlighted())
				return;

			var areaHeight = this.itemsContainer.scrollRect.height;
			var totalHeight = this._itemHeight * this._itemArr.length;

			if (areaHeight == 0 || totalHeight == 0)
				return;

			var percent:Number = evn.localY / areaHeight;
			var scrollPos:Number = percent * totalHeight;
			var thumbHeight:Number = areaHeight / totalHeight * areaHeight;
			var thumbPercent:Number = thumbHeight / areaHeight;
			scrollPos -= thumbPercent * totalHeight / 2;
			setScroll(scrollPos, true);
		}
	}
}