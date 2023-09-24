package UIComponent {
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.filters.DropShadowFilter;

	public class Label extends TextField implements IComponent {		
		private var _width:Number;
		private var _height:Number;

		private var _text:String;
		private var _fontSize:Number;
		private var _defaultTextFormat:TextFormat;

		public function Label(width:Number, height:Number, text:String, fontSize:Number) {
			super();

			this._width = width;
			this._height = height;

			this._text = text;
			this._fontSize = fontSize;

			initializeComponent();
		}

		public function Activate() : void {}

		public function CanChangeFocus(dir:int) : Boolean {
			return true;
		}

		public function IsFocusable() : Boolean {
			return false;
		}

		public function SetFocus(onOff:Boolean) : void {}

		public function ProcessKeyEvent(keyCode:uint) : void {}

		private function initializeComponent() {
			var _defaultTextFormat:TextFormat = new TextFormat(Shared.MainFont.fontName, this._fontSize, Shared.Color_Primary);
			_defaultTextFormat.kerning = true;
			_defaultTextFormat.align = "center";

			this.selectable = false;
			this.embedFonts = true;
			this.defaultTextFormat = _defaultTextFormat;
			this.filters = [new DropShadowFilter(0.5, 45, Shared.Color_Primary, 0.6, 0, 0, 1.0, 0.3, false, false, false)];
			this.width = this._width;
			this.height = this._height;
			this.text = this._text;
		}
	}
}