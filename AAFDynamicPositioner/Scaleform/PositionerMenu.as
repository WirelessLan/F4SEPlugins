package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.Slider;
	import UIComponent.UIEvent;
	
	public class PositionerMenu extends View {
		private var menuWidth:uint = 560;
		private var menuHeight:uint = 245;
				
		private var _sliderX:Slider;
		private var _sliderY:Slider;
		private var _sliderZ:Slider;
		
		private var _sliderXLbl:Label;
		private var _sliderYLbl:Label;
		private var _sliderZLbl:Label;

		public function PositionerMenu(x:Number, y:Number, z:Number) {
        	super(menuWidth, menuHeight, "액터 위치 조절");
			
			this._sliderX.value = x;
			this._sliderY.value = y;
			this._sliderZ.value = z;
		}
		
		public function UpdateSlider(x:Number, y:Number, z:Number) {
			if (this.focusedComponent) {
				this.focusedComponent.SetFocus(false);
				this.focusedComponent = null;
			}
			
			this._sliderX.value = x;
			this._sliderY.value = y;
			this._sliderZ.value = z;
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			this.x = 700;
			this.y = 450;
			
			var sliderNameLbl:Label = new Label(menuWidth - 20, 24, "X축", 16);
			sliderNameLbl.x = 15;
			sliderNameLbl.y = 40;
			this.AddComponent(sliderNameLbl);
			
			this._sliderX = new Slider(menuWidth - 100, 30, -200, 200, 0.1);
			this._sliderX.x = 15;
			this._sliderX.y = 65;
			this._sliderX.addEventListener(UIEvent.CHANGE, _sliderX_change);
			this.AddComponent(this._sliderX);
			
			this._sliderXLbl = new Label(80, 30, "0.0", 24, "center");
			this._sliderXLbl.x = 475;
			this._sliderXLbl.y = 62;
			this.AddComponent(this._sliderXLbl);
			
			sliderNameLbl = new Label(menuWidth - 20, 24, "Y축", 16);
			sliderNameLbl.x = 15;
			sliderNameLbl.y = 105;
			this.AddComponent(sliderNameLbl);
			
			this._sliderY = new Slider(menuWidth - 100, 30, -200, 200, 0.1);
			this._sliderY.x = 15;
			this._sliderY.y = 130;
			this._sliderY.addEventListener(UIEvent.CHANGE, _sliderY_change);
			this.AddComponent(this._sliderY);
			
			this._sliderYLbl = new Label(80, 30, "0.0", 24, "center");
			this._sliderYLbl.x = 475;
			this._sliderYLbl.y = 132;
			this.AddComponent(this._sliderYLbl);
			
			sliderNameLbl = new Label(menuWidth - 20, 24, "Z축", 16);
			sliderNameLbl.x = 15;
			sliderNameLbl.y = 170;
			this.AddComponent(sliderNameLbl);
			
			this._sliderZ = new Slider(menuWidth - 100, 30, -200, 200, 0.1);
			this._sliderZ.x = 15;
			this._sliderZ.y = 195;
			this._sliderZ.addEventListener(UIEvent.CHANGE, _sliderZ_change);
			this.AddComponent(this._sliderZ);
			
			this._sliderZLbl = new Label(80, 30, "0.0", 24, "center");
			this._sliderZLbl.x = 475;
			this._sliderZLbl.y = 197;
			this.AddComponent(this._sliderZLbl);
		}
		
		private function _sliderX_change(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SetPosition("X", evn.value);
			this._sliderXLbl.text = evn.value.toFixed(1);
		}
		
		private function _sliderY_change(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SetPosition("Y", evn.value);
			this._sliderYLbl.text = evn.value.toFixed(1);
		}
		
		private function _sliderZ_change(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SetPosition("Z", evn.value);
			this._sliderZLbl.text = evn.value.toFixed(1);
		}
	}
}