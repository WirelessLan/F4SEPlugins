package UIComponent {
	import flash.display.Sprite;
   
	public class Scrollbar extends Sprite {
		private var thumbSize:Number;
		private var areaHeight:Number;
		
		private var thumb:Sprite;
		
		public function Scrollbar(areaHeight:Number, totalHeight:Number) {
			super();
			
			if(totalHeight > areaHeight) {
				this.thumbSize = areaHeight / totalHeight * areaHeight;
				this.areaHeight = areaHeight;
				this.thumb = new Sprite();
				this.thumb.graphics.beginFill(Shared.Color_Primary, 0.8);
				this.thumb.graphics.drawRect(0, 0, 5, this.thumbSize);
				this.thumb.graphics.endFill();
				this.addChild(this.thumb);
				return;
			}
		}
      
		public function setScrollPercent(param1:Number) : void {
			var _loc2_:Number = this.areaHeight - this.thumbSize;
			if (this.thumb)
				this.thumb.y = _loc2_ / 100 * param1;
		}
	}
}