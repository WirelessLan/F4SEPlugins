package UIComponent {
	import flash.text.TextField;
	import flash.text.TextFormat;

	public class Label extends TextField implements IComponent {		
		private var _width:Number;
		private var _height:Number;
		
		private var _text:String;
		private var _fontSize:Number;
		private var _defaultTextFormat:TextFormat;
		
		public function Label(width:Number, height:Number, text:String, fontSize:Number) {
			this._width = width;
			this._height = height;
			this._text = text;
			this._fontSize = fontSize;
			initializeComponent();
		}
		
		public function Activate() : void {
		}
		
		public function CanChangeFocus(dir:int) : Boolean {
			return true;
		}
		
		public function IsFocusable() : Boolean {
			return false;
		}
		
		public function SetFocus(onOff:Boolean, dir:int) : void {
		}
		
		public function SetInnerFocus(dir:int) : void {
		}
		
		private function initializeComponent() {
			var _defaultTextFormat:TextFormat = new TextFormat(Shared.MainFont.fontName, this._fontSize, Shared.Color_Primary);
			_defaultTextFormat.kerning = true;
			_defaultTextFormat.align = "center";
			
			this.selectable = false;
			this.embedFonts = true;
			this.defaultTextFormat = _defaultTextFormat;
			this.width = this._width;
			this.height = this._height;
			this.text = this._text;
		}
	}
}