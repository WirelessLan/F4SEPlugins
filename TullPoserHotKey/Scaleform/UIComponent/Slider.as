package UIComponent {
	import flash.display.Sprite;
	import flash.geom.Rectangle;
	import flash.geom.Point;
	import flash.events.MouseEvent;
	import flash.ui.Keyboard;

	public class Slider extends Sprite implements IComponent {		
		private var _width:Number;
		private var _height:Number;
		
		private var _focused:Boolean = false;
		private var _focusColor:Number = 0x808080;
		
		private var _dragMode:Boolean = false;
		private var _barWidth:Number = 10;
		private var _barPos:Number;
		private var _margin:Number = 5;
		private var _min:Number;
		private var _max:Number;
		private var _unit:Number = 1;
		private var _value:Number;

		public function Slider(width:Number, height:Number, min:Number, max:Number, unit:Number) {
			this._width = width;
			this._height = height;
			this._min = min;
			this._max = max;
			this._unit = unit;
			this._value = min;
			initializeComponent();
		}
		
		public function Activate() : void {}
		
		public function CanChangeFocus(dir:int) : Boolean {
			return true;
		}
		
		public function IsFocusable() : Boolean {
			return true;
		}
		
		public function SetFocus(onOff:Boolean) : void {
			this._focused = onOff;
			if (!onOff)
				this._dragMode = false;
			draw();
		}
		
		public function SetInnerFocus(dir:int) : void {}
		
		public function ProcessKeyEvent(keyCode:uint) : void {
			if (keyCode == Keyboard.LEFT)
				decValue();
			else if (keyCode == Keyboard.RIGHT)
				incValue();
		}
		
		public function set value(value:Number) : void {
			if (compareNumber(this._value, value, this._unit) == 0)
				return;
				
			this._value = roundOff(value, this._unit);
			sendEvent();
			setBarPosByValue();
			draw();
		}
		
		public function get value() : Number {
			return this._value;
		}
		
		private function setBarPosByValue() : void {
			var gap = this._margin + this._barWidth / 2;
			var range = (this._max - this._min) / this._unit;
			var wPr = (this._width - gap * 2) / range;
			this._barPos = gap + wPr * ((this._value - this._min) / this._unit);
		}
		
		private function sendEvent() : void {
			this.dispatchEvent(new UIEvent(UIEvent.CHANGE, this._value));
		}
		
		private function incValue() : void {
			if (compareNumber(this._value, this._max, this._unit) < 0)
				value = this._value + this._unit;
		}
		
		private function decValue() : void {
			if (compareNumber(this._value, this._min, this._unit) > 0)
				value = this._value - this._unit;
		}
		
		private function roundOff(n:Number, dp:Number) : Number {
			if (n >= 0)
				return int((n + dp / 2) / dp) * dp;
			else
				return int((n - dp / 2) / dp) * dp;
		}
		
		private function compareNumber(n1:Number, n2:Number, dp:Number) : int {
			var iN1 = n1 >= 0 ? int((n1 + dp / 2) / dp) : int((n1 - dp / 2) / dp);
			var iN2 = n2 >= 0 ? int((n2 + dp / 2) / dp) : int((n2 - dp / 2) / dp);
			return iN1 - iN2;
		}
		
		private function calculatePos(pos:Number) : Number {
			var gap = this._margin + this._barWidth / 2;
			var range = (this._max - this._min) / this._unit;
			var wPr = (this._width - gap * 2) / range;
			var posIdx:uint = 0;
			for (; posIdx < range; posIdx++) {
				if (gap + wPr * (posIdx + 1) < pos)
					continue;
					
				if (pos - (gap + wPr * posIdx) < (gap + wPr * (posIdx + 1)) - pos) {
					break;
				}
				else {
					posIdx++;
					break;
				}
			}
			
			var barPos = gap + wPr * posIdx;
			var value = this._min + posIdx * this._unit;
			if (compareNumber(this._value, value, this._unit) == 0)
				return barPos;
			
			this.value = value;
			sendEvent();
			return barPos;
		}
		
		private function setPosByCursor() : void {
			var gap = this._margin + this._barWidth / 2;
			var newX = this.globalToLocal(new Point(stage.mouseX, 0)).x;
			if (newX < gap)
				newX = gap;
			else if (newX > this._width - gap)
				newX = this._width - gap;
				
			var calPos = calculatePos(newX);
			if (compareNumber(this._barPos, calPos, 0.01))
				return;
				
			this._barPos = calPos;
			draw();
		}
		
		private function stage_onMouseMove(evn:MouseEvent) : * {
			if (!this._dragMode)
				return;
				
			if (!evn.buttonDown) {
				this._dragMode = false;
				return;
			}
			
			setPosByCursor();
		}
		
		private function onMouseDown(evn:MouseEvent) : * {
			this._dragMode = true;
			stage.addEventListener(MouseEvent.MOUSE_MOVE, stage_onMouseMove);
		}
		
		private function onMouseUp(evn:MouseEvent) : * {
			if (!this._dragMode)
				return;
				
			this._dragMode = false;
			stage.removeEventListener(MouseEvent.MOUSE_MOVE, stage_onMouseMove);
			
			var gap = this._margin + this._barWidth / 2;
			var newX = this.globalToLocal(new Point(stage.mouseX, 0)).x;
			if (newX < gap)
				newX = gap;
			else if (newX > this._width - gap)
				newX = this._width - gap;
				
			var calPos = calculatePos(newX);
			if (compareNumber(this._barPos, calPos, 0.01))
				return;
				
			this._barPos = calPos;
			draw();
		}
		
		private function initializeComponent() : void {
			this.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
			this.addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
			setBarPosByValue();
			draw();
		}
		
		private function draw() {
			this.graphics.clear();
			if (this._focused)
				this.graphics.lineStyle(1, this._focusColor);
			else
				this.graphics.lineStyle(1, Shared.Color_Primary);
			this.graphics.beginFill(Shared.Color_Background, 1);
			this.graphics.drawRect(0, 0, this._width, this._height);
			this.graphics.endFill();
			
			if (this._focused)
				this.graphics.beginFill(this._focusColor, 1);
			else
				this.graphics.beginFill(Shared.Color_Primary, 1);
			this.graphics.drawRect(this._barPos - (this._barWidth / 2), 5, this._barWidth, this._height - 10);
			this.graphics.endFill();
		}
	}
}