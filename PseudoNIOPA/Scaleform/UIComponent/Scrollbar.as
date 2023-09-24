﻿package UIComponent {
	import flash.display.Sprite;
	import flash.events.MouseEvent;

	public class Scrollbar extends Sprite {
		private var _width:Number;
		private var _height:Number;

		private var _highlightThumbColor:Number = 0xA6A6A6;
		private var _dragThumbColor:Number = 0x5A5A5A;
		private var _isHighlighted:Boolean = false;
		private var _isDragging:Boolean = false;
		private var _thumbPos:Number;
		private var _thumbHeight:Number;

		private var scrollContainer:Sprite;
		private var scrollThumb:Sprite;		

		public function Scrollbar(width:Number, height:Number) {
			super();

			this._width = width;
			this._height = height;

			this._thumbPos = 0;
			this._thumbHeight = 0;

			initializeComponent();
		}

		public function IsHighlighted() : Boolean {
			return this._isHighlighted;
		}

		public function IsDragging() : Boolean {
			return this._isDragging;
		}

		public function SetThumbHeight(areaHeight:Number, totalHeight:Number) : void {
			if (totalHeight == 0)
				return;

			var thumbHeight = areaHeight / totalHeight * areaHeight;
			if (thumbHeight >= totalHeight)
				return;

			this._thumbHeight = thumbHeight;
			drawThumb();
		}

		public function SetThumbPosition(percent:Number) : void {				
			var yPos = (this._height - this._thumbHeight) / 100 * percent;
			this._thumbPos = yPos;
			drawThumb();
		}

		private function initializeComponent() {
			this.scrollContainer = new Sprite();
			this.scrollContainer.graphics.beginFill(0xFFFFFF, 0);
			this.scrollContainer.graphics.drawRect(0, 0, this._width, this._height);
			this.scrollContainer.graphics.endFill();
			this.addChild(this.scrollContainer);

			this.scrollThumb = new Sprite();
			this.scrollThumb.addEventListener(MouseEvent.MOUSE_OVER, onMouseOver);
			this.scrollThumb.addEventListener(MouseEvent.MOUSE_OUT, onMouseOut);
			this.scrollThumb.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
			this.addChild(this.scrollThumb);

			drawThumb();
		}

		private function drawThumb() {
			this.scrollThumb.graphics.clear();
			if (this._isDragging)
				this.scrollThumb.graphics.beginFill(this._dragThumbColor, 1);
			else if (this._isHighlighted)
				this.scrollThumb.graphics.beginFill(this._highlightThumbColor, 1);
			else
				this.scrollThumb.graphics.beginFill(Shared.Color_Primary, 1);
			this.scrollThumb.graphics.drawRect(0, this._thumbPos, this._width, this._thumbHeight);
			this.scrollThumb.graphics.endFill();
		}

		private function onMouseOver(evn:MouseEvent) : void {
			this._isHighlighted = true;
			drawThumb();
		}

		private function onMouseOut(evn:MouseEvent) : void {
			this._isHighlighted = false;
			drawThumb();
		}

		private function onMouseDown(evn:MouseEvent) : void {
			stage.addEventListener(MouseEvent.MOUSE_UP, stage_onMouseUp);
			this._isDragging = true;
			drawThumb();
		}

		private function stage_onMouseUp(evn:MouseEvent) : void {
			stage.removeEventListener(MouseEvent.MOUSE_UP, stage_onMouseUp);
			this._isDragging = false;
			drawThumb();
		}
	}
}