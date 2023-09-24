package UIComponent {
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.filters.DropShadowFilter;
	import flash.events.MouseEvent;

	public class Button extends TextField implements IComponent {		
		private var _width:Number;
		private var _height:Number;

		private var _fontSize:Number = 20;

		public function Button(width:Number, height:Number) {
			super();

			this._width = width;
			this._height = height;

			initializeComponent();
		}

		public function Activate() : void {
			this.dispatchEvent(new UIEvent(UIEvent.ACTIVATE));
		}

		public function CanChangeFocus(dir:int) : Boolean {
			return true;
		}

		public function IsFocusable() : Boolean {
			return true;
		}

		public function SetFocus(onOff:Boolean) : void {
			focusButton(onOff);
		}

		public function ProcessKeyEvent(keyCode:uint) : void {}

		private function focusButton(onOff:Boolean) : void {
			var this_tf:TextFormat = this.getTextFormat();

			if (onOff) {
				this.background = true;
				this.backgroundColor = Shared.Color_Primary;

				this_tf.color = Shared.Color_Secondary;
			}
			else {
				this.background = false;
				this.backgroundColor = Shared.Color_DefaultBackground;

				this_tf.color = Shared.Color_Primary;
			}

			this.setTextFormat(this_tf);
		}

		private function onButtonClick(evn:MouseEvent) : * {
			this.Activate();
		}

		private function initializeComponent() {
			var this_tf:TextFormat = new TextFormat(Shared.MainFont.fontName, this._fontSize, Shared.Color_Primary);
			this_tf.kerning = true;

			this.selectable = false;
			this.embedFonts = true;
			this.defaultTextFormat = this_tf;
			this.filters = [new DropShadowFilter(0.5, 45, Shared.Color_Primary, 0.6, 0, 0, 1.0, 0.3, false, false, false)];
			this.width = this._width;
			this.height = this._height;
			this.addEventListener(MouseEvent.CLICK, onButtonClick);
		}
	}
}