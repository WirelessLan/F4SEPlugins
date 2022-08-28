package UIComponent {
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.events.MouseEvent;

	public class Button extends TextField implements IComponent {		
		private var _width:Number;
		private var _height:Number;
		
		public function Button(width:Number, height:Number) {
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
		
		public function SetFocus(onOff:Boolean, dir:int) : void {
			focusButton(onOff);
		}
		
		public function SetInnerFocus(dir:int) : void {
		}
		
		public override function set text(str:String) : void {
			super.text = str;
			
			var this_tf:TextFormat = this.getTextFormat();
			this_tf.color = 0xFFFFFF;
			this_tf.font = Shared.MainFont.fontName;
			this_tf.size = 24;
			this_tf.kerning = true;
			this_tf.align = "center";
			
			this.setTextFormat(this_tf);
		}
		
		private function focusButton(onOff:Boolean) : void {
			var this_tf:TextFormat = this.getTextFormat();
			
			if (onOff) {
				this.background = true;
				this.backgroundColor = 0xFFFFFF;
			
				this_tf.color = 0x000000;
			}
			else {
				this.background = false;
				this.backgroundColor = 0x000000;
				
				this_tf.color = 0xFFFFFF;
			}
			
			this.setTextFormat(this_tf);
		}
		
		private function onButtonClick(evn:MouseEvent) : * {
			this.Activate();
		}
		
		private function initializeComponent() {
			this.selectable = false;
			this.embedFonts = true;
			this.width = this._width;
			this.height = this._height;
			this.border = true;
			this.borderColor = 0xFFFFFF;
			
			var this_tf:TextFormat = this.getTextFormat();
			this_tf.color = 0xFFFFFF;
			this_tf.font = Shared.MainFont.fontName;
			this_tf.size = 24;
			this_tf.kerning = true;
			this_tf.align = "center";
			
			this.setTextFormat(this_tf);
			this.addEventListener(MouseEvent.CLICK, onButtonClick);
		}
	}
}