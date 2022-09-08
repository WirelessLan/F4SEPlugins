package UIComponent {
	import flash.display.Sprite;
	import flash.geom.Rectangle;
	import flash.events.MouseEvent;
	import flash.display.DisplayObject;
	import flash.ui.Keyboard;
	
	public class View extends BaseContainer {
		private var titleLbl:Label;
		
		private var _title:String;

		public function View(width:Number, height:Number, title:String) {
			super(width, height);
			this._title = title;
			
			InitializeView();
		}
		
		protected function InitializeView() : void {
			this.x = (Shared.UIWidth - this._width) / 2;
			this.y = (Shared.UIHeight - this._height) / 2;
			
			this.graphics.beginFill(Shared.Color_Background, 0.7);
			this.graphics.drawRect(0, 0, this._width, this._height);
			this.graphics.endFill();
			
			var unlineWidth:uint = 180;
			this.graphics.lineStyle(2, Shared.Color_Primary);
			this.graphics.moveTo((this._width - unlineWidth) / 2, 45);
			this.graphics.lineTo((this._width - unlineWidth) / 2 + unlineWidth, 45);
			
			titleLbl = new Label(this._width, 35, this._title, 25);
			titleLbl.x = 0;
			titleLbl.y = 5;
			this.AddComponent(titleLbl);
		}
	}
}