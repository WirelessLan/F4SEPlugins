package UIComponent {
	import flash.display.Sprite;
	import flash.display.DisplayObject;
	import flash.geom.Rectangle;
	import flash.events.MouseEvent;
	import flash.ui.Keyboard;

	public class Panel extends BaseContainer implements IComponent {
		private var scrollbar:Scrollbar;
		
		public function Panel(width:Number, height:Number) {
			super(width, height);			
			initializeComponent();
		}
		
		public function Activate() : void {}
		
		public function CanChangeFocus(dir:int) : Boolean {
			if (this.componentList.length == 0)
				return true;

			if (!this.focusedComponent)
				return false;
			
			if (dir == FocusDirection.Previous && this.focusedComponent == FindFirstFocusableComponent())
				return true;
			else if (dir == FocusDirection.Next && this.focusedComponent == FindLastFocusableComponent())
				return true;
			return false;
		}
		
		public function IsFocusable() : Boolean {
			if (this.numChildren == 0)
				return false;
			return true;
		}
		
		public function SetFocus(onOff:Boolean) : void {
			if (!onOff && this.focusedComponent) {
				this.focusedComponent.SetFocus(false);
				this.focusedComponent = null;
			}
		}
		
		public override function ProcessKeyEvent(keyCode:uint) : void {
			super.ProcessKeyEvent(keyCode);
			
			if (this.focusedComponent) {
				var com = this.focusedComponent as DisplayObject;
				setScroll(calculateScrollPosition(com));
			}
		}
		
		public override function AddComponent(component:DisplayObject) : void {
			super.AddComponent(component);
			
			if (this.scrollbar)
				this.removeChild(this.scrollbar);
			
			this.scrollbar = new Scrollbar(this.componentContainer.scrollRect.height, this.componentContainer.getChildAt(this.componentContainer.numChildren - 1).y + 20);
			this.scrollbar.x = this.componentContainer.scrollRect.width - this.scrollbar.width ;
			this.scrollbar.y = 0;
			
			this.addChild(this.scrollbar);
		}
      
		private function get maxScrollOffset() : Number	{
			var maxY:Number = 0;
			var com:DisplayObject;
			for (var ii = 0; ii < this.componentList.length; ii++) {
				com = this.componentList[ii] as DisplayObject;
				if (maxY < com.y + com.height)
					maxY = com.y + com.height;
			}
			
			return maxY > this.componentContainer.scrollRect.height ? 
				Number(maxY - this.componentContainer.scrollRect.height + 10) : Number(0);
		}
		
		private function calculateScrollPosition(com:DisplayObject) : Number {
			if (!com)
				return this.componentContainer.scrollRect.y;

			if (com.y < this.componentContainer.scrollRect.y)
				return com.y;
			else if (com.y + com.height > this.componentContainer.scrollRect.y + this.componentContainer.scrollRect.height)
				return com.y - this.componentContainer.scrollRect.height + com.height;
			return this.componentContainer.scrollRect.y;
		}
		
		private function onMouseWheel(evn:MouseEvent) : * {
			var wheel_d:int = evn.delta;
			var currY:Number = this.componentContainer.scrollRect.y;
			
			if (wheel_d <= 0 && currY < this.maxScrollOffset) {
				currY -= wheel_d * 30;
				if (currY > this.maxScrollOffset)
					currY = this.maxScrollOffset;
			}
			else if (wheel_d > 0 && currY > 0) {
				currY -= wheel_d * 30;
				if (currY < 0)
					currY = 0;
			}
			
			setScroll(currY);
		}
		
		private function setScroll(newY:Number) : void {
			if (this.maxScrollOffset == 0)
				return;
				
			if (newY < 0)
				newY = 0;
				
			if (newY > this.maxScrollOffset)
				newY = this.maxScrollOffset;
			
			if (this.componentContainer.scrollRect.y == newY)
				return;
			
			var sRect:Rectangle = this.componentContainer.scrollRect;
			sRect.y = newY;
			this.componentContainer.scrollRect = sRect;
			var scrollPer:Number = 100 * sRect.y / this.maxScrollOffset;
			this.scrollbar.setScrollPercent(scrollPer);
		}
		
		private function initializeComponent() {
			this.graphics.beginFill(0xFFFFFF, 0);
			this.graphics.drawRect(0, 0, width, height);
			this.graphics.endFill();
			this.graphics.lineStyle(1, Shared.Color_Primary);
			this.graphics.drawRect(0, 0, width, height);
			this.addEventListener(MouseEvent.MOUSE_WHEEL, onMouseWheel);
			
			this.componentContainer.cacheAsBitmap = true;
			this.componentContainer.scrollRect = new Rectangle(0, 0, width, height);
		}
	}
}