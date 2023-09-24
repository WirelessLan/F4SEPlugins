package UIComponent {
	import flash.display.Sprite;
	import flash.display.DisplayObject;
	import flash.geom.Rectangle;
	import flash.ui.Keyboard;
	import flash.events.MouseEvent;

	public class View extends BaseContainer {
		private var _title:String;

		private var titleLbl:Label;
		private var backBtn:WindowButton;
		private var closeBtn:WindowButton;

		public function View(width:Number, height:Number, title:String) {
			super(width, height);

			this._title = title;

			InitializeView();
		}

		protected function InitializeView() : void {
			this.x = (Shared.UIWidth - this._width) / 2;
			this.y = (Shared.UIHeight - this._height) / 2;

			this.graphics.beginFill(Shared.Color_Background, 0.8);
			this.graphics.drawRect(0, 45, this._width, this._height);
			this.graphics.endFill();

			this.graphics.lineStyle(2, Shared.Color_Primary);
			this.graphics.drawRect(0, 45, this._width, this._height);

			this.titleLbl = new Label(this._width, 38, this._title, 30);
			this.titleLbl.x = 0;
			this.titleLbl.y = 5;
			this.AddComponent(this.titleLbl);

			if (Shared.Root.numChildren > 0) {
				this.backBtn = new WindowButton(30, 30);
				this.backBtn.x = 0;
				this.backBtn.y = 10;
				this.backBtn.graphics.lineStyle(4, Shared.Color_Primary);
				this.backBtn.graphics.moveTo(this.backBtn.width / 2, 5);
				this.backBtn.graphics.lineTo(5, this.backBtn.height / 2);
				this.backBtn.graphics.lineTo(this.backBtn.width / 2, this.backBtn.height - 5);
				this.backBtn.addEventListener(UIEvent.ACTIVATE, backBtn_Activated);
				this.backBtn.addEventListener(MouseEvent.ROLL_OVER, ViewBtn_MouseOver);
				this.backBtn.addEventListener(MouseEvent.ROLL_OUT, ViewBtn_MouseOut);
				this.AddComponent(this.backBtn);
			}

			this.closeBtn = new WindowButton(30, 30);
			this.closeBtn.x = this._width - 30;
			this.closeBtn.y = 10;
			this.closeBtn.graphics.lineStyle(4, Shared.Color_Primary);
			this.closeBtn.graphics.moveTo(5, 5);
			this.closeBtn.graphics.lineTo(this.closeBtn.width - 5, this.closeBtn.height - 5);
			this.closeBtn.graphics.moveTo(this.closeBtn.width - 5, 5);
			this.closeBtn.graphics.lineTo(5, this.closeBtn.height - 5);
			this.closeBtn.addEventListener(UIEvent.ACTIVATE, closeBtn_Activated);
			this.closeBtn.addEventListener(MouseEvent.ROLL_OVER, ViewBtn_MouseOver);
			this.closeBtn.addEventListener(MouseEvent.ROLL_OUT, ViewBtn_MouseOut);
			this.AddComponent(this.closeBtn);
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