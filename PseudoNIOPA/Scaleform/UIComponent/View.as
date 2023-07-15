package UIComponent {
	import flash.display.Sprite;
	import flash.display.DisplayObject;
	import flash.geom.Rectangle;
	import flash.ui.Keyboard;
	import flash.events.MouseEvent;
	
	public class View extends BaseContainer {
		private var _title:String;
		
		private var titleLbl:Label;
		private var backBtn:Button;
		private var closeBtn:Button;
		
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
			
			this.titleLbl = new Label(this._width, 38, this._title, 30);
			this.titleLbl.x = 0;
			this.titleLbl.y = 5;
			this.AddComponent(this.titleLbl);
			
			if (Shared.Root.numChildren > 0) {
				this.backBtn = new Button(30, 30);
				this.backBtn.x = 10;
				this.backBtn.y = 10;
				this.backBtn.text = "<<";
				this.backBtn.addEventListener(UIEvent.ACTIVATE, backBtn_Activated);
				this.backBtn.addEventListener(MouseEvent.ROLL_OVER, ViewBtn_MouseOver);
				this.backBtn.addEventListener(MouseEvent.ROLL_OUT, ViewBtn_MouseOut);
				this.componentContainer.addChild(this.backBtn);
			}

			this.closeBtn = new Button(30, 30);
			this.closeBtn.x = this._width - 40;
			this.closeBtn.y = 10;
			this.closeBtn.text = "X";
			this.closeBtn.addEventListener(UIEvent.ACTIVATE, closeBtn_Activated);
			this.closeBtn.addEventListener(MouseEvent.ROLL_OVER, ViewBtn_MouseOver);
			this.closeBtn.addEventListener(MouseEvent.ROLL_OUT, ViewBtn_MouseOut);
			this.componentContainer.addChild(this.closeBtn);
		}
		
		private function ViewBtn_MouseOver(evn:MouseEvent) : * {
			if (!evn.currentTarget is IComponent)
				return;
				
			var comp:IComponent = evn.currentTarget as IComponent;
			comp.SetFocus(true);
		}
		
		private function ViewBtn_MouseOut(evn:MouseEvent) : * {
			if (!evn.currentTarget is IComponent)
				return;
				
			var comp:IComponent = evn.currentTarget as IComponent;
			comp.SetFocus(false);
		}
		
		private function backBtn_Activated(evn:UIEvent) : * {
			Shared.CloseView(1);
		}
		
		private function closeBtn_Activated(evn:UIEvent) : * {
			Shared.CloseView(0);
		}
	}
}