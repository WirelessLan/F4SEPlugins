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
			
			this.graphics.beginFill(Shared.Color_Background, 0.8);
			this.graphics.drawRect(0, 0, this._width, this._height);
			this.graphics.endFill();
			
			this.graphics.lineStyle(2, Shared.Color_Primary);
			this.graphics.drawRect(0, 0, this._width, this._height);
			
			var unlineWidth:uint = this._width - 20;
			this.graphics.lineStyle(1, Shared.Color_Primary);
			this.graphics.moveTo((this._width - unlineWidth) / 2, 45);
			this.graphics.lineTo((this._width - unlineWidth) / 2 + unlineWidth, 45);
			
			titleLbl = new Label(this._width, 38, this._title, 30);
			titleLbl.x = 0;
			titleLbl.y = 5;
			this.AddComponent(titleLbl);
		}
		
		private function onMouseOver(evn:MouseEvent) : * {
			evn.stopPropagation();
			
			if (!evn.currentTarget is IComponent)
				return;
				
			var tmpComp:IComponent = evn.currentTarget as IComponent;
			if (tmpComp == this.focusedComponent || !tmpComp.IsFocusable())
				return;
				
			if (this.focusedComponent)
				this.focusedComponent.SetFocus(false);
			this.focusedComponent = evn.currentTarget as IComponent;
			this.focusedComponent.SetFocus(true);
		}
	}
}