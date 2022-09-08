package UIComponent {
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.text.TextFieldType;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.ui.Keyboard;

	public class TextInput extends TextField implements IComponent {		
		private var _width:Number;
		private var _height:Number;

		private var _isEditState:Boolean = false;
		private var _enteredEditStateByActivate:Boolean = false;
		private var _fontSize:Number;
		private var _placeHolder:String;
		private var _isFocused = false;
		
		public function TextInput(width:Number, height:Number, fontSize:Number) {
			this._width = width;
			this._height = height;
			this._fontSize = fontSize;
			initializeComponent();
		}
		
		public function Activate() : void {
			if (!this._isEditState) {
				_enteredEditStateByActivate = true;
				setEditState(true);
			}
			else {
				setEditState(false);
			}
		}
		
		public function CanChangeFocus(dir:int) : Boolean {
			return true;
		}
		
		public function IsFocusable() : Boolean {
			return true;
		}
		
		public function SetFocus(onOff:Boolean) : void {
			setFocus(onOff);
			if (!onOff && this._isEditState) {
				setEditState(false);
			}
		}
		
		public function ProcessKeyEvent(keyCode:uint) : void {}
		
		private function setFocus(onOff:Boolean) : void {
			var this_tf:TextFormat = this.getTextFormat();
			
			if (onOff) {
				this.background = true;
				this.backgroundColor = Shared.Color_Background;
				this._isFocused = true;
			}
			else {
				this.background = false;
				this.backgroundColor = 0xFFFFFF;
				this._isFocused = false;
			}
			
			this.setTextFormat(this_tf);
		}
		
		private function onKeyUp(evn:KeyboardEvent) : * {
			if (!this._isEditState)
				return;
				
			if (this._enteredEditStateByActivate) {
				this._enteredEditStateByActivate = false;
				return;
			}
				
			if (evn.keyCode == Keyboard.ENTER || evn.keyCode == Keyboard.TAB
			 || evn.keyCode == Keyboard.UP || evn.keyCode == Keyboard.DOWN) {
				setEditState(false);
			}
		}
		
		private function setEditState(onOff:Boolean) : * {
			if (onOff) {
				this._isEditState = true;
				if (Shared.F4SEPlugin)
					Shared.F4SEPlugin.AllowTextInput(true);
				this.type = TextFieldType.INPUT;
				
				this.background = true;
				this.backgroundColor = Shared.Color_DefaultBackground;
				this.textColor = Shared.Color_Secondary;
				
				stage.focus = this;
			}
			else {
				this._isEditState = false;
				if (Shared.F4SEPlugin)
					Shared.F4SEPlugin.AllowTextInput(false);
				this.type = TextFieldType.DYNAMIC;
				
				if (stage.focus == this)
					stage.focus = null;
				
				if (this._isFocused) {
					this.background = true;
					this.backgroundColor = Shared.Color_Background;
				}
				else {
					this.background = false;
					this.backgroundColor = Shared.Color_DefaultBackground;
				}
				
				this.textColor = Shared.Color_Primary;
			}
		}
		
		private function onClick(evn:MouseEvent) : * {
			if (!this._isEditState)
				setEditState(true);
			else
				setEditState(false);
		}
		
		private function initializeComponent() {
			var this_tf:TextFormat = new TextFormat();
			this_tf.color = Shared.Color_Primary;
			this_tf.font = Shared.MainFont.fontName;
			this_tf.size = this._fontSize;
			this_tf.kerning = true;
			this_tf.align = "center";
			
			this.setTextFormat(this_tf);
			this.selectable = true;
			this.embedFonts = true;
			this.wordWrap = false;
			this.defaultTextFormat = this_tf;
			this.width = this._width;
			this.height = this._height;
			this.border = true;
			this.borderColor = Shared.Color_Primary;
			this.addEventListener(KeyboardEvent.KEY_UP, onKeyUp);
			this.addEventListener(MouseEvent.CLICK, onClick);
		}
	}
}