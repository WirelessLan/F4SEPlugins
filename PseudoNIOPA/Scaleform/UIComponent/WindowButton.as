package UIComponent {
	import flash.display.DisplayObject;
	import flash.display.Sprite;
	import flash.geom.ColorTransform;
	import flash.filters.DropShadowFilter;
	import flash.events.MouseEvent;

	public class WindowButton extends Sprite implements IComponent {		
		private var _width:Number;
		private var _height:Number;

		public function WindowButton(width:Number, height:Number) {
			super();

			this._width = width;
			this._height = height;

			initializeComponent();
		}

		public function Activate() : void {
			this.dispatchEvent(new UIEvent(UIEvent.ACTIVATE));
		}

		public function CanChangeFocus(dir:int) : Boolean {
			return false;
		}

		public function IsFocusable() : Boolean {
			return false;
		}

		public function SetFocus(onOff:Boolean) : void {}

		public function ProcessKeyEvent(keyCode:uint) : void {}

		private function onMouseOver(evn:MouseEvent) : void {
			transformColor(this, Shared.Color_DefaultBackground);
		}

		private function onMouseOut(evn:MouseEvent) : void {
			transformColor(this, Shared.Color_Primary);
		}

		function transformColor(target:DisplayObject, color:uint) : void {
			var colorTransform:ColorTransform = new ColorTransform();
			colorTransform.color = color;
			target.transform.colorTransform = colorTransform;
		}

		private function onButtonClick(evn:MouseEvent) : * {
			this.Activate();
		}

		private function initializeComponent() {
			this.graphics.beginFill(Shared.Color_Primary, 0);
			this.graphics.drawRect(0, 0, this._width, this._height);
			this.graphics.endFill();

			this.addEventListener(MouseEvent.MOUSE_OVER, onMouseOver);
			this.addEventListener(MouseEvent.MOUSE_OUT, onMouseOut);
			this.addEventListener(MouseEvent.CLICK, onButtonClick);
		}
	}
}