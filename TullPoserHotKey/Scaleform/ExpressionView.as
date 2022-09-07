package  {
	import UIComponent.View;
	import UIComponent.Panel;
	import UIComponent.Label;
	import UIComponent.Slider;
	import UIComponent.Button;
	import UIComponent.UIEvent;
	
	public class ExpressionView extends View {
		private var menuWidth:uint = 320;
		private var menuHeight:uint = 680;
		
		private var _sliderInitValArr:Array;
		
		private var panel:Panel;
		private var sliderArr:Array;
		private var sliderValueLblArr:Array;
		private var loadBtn:Button;
		private var saveBtn:Button;
		private var resetBtn:Button;

		public function ExpressionView(expArr:Array) {
			this._sliderInitValArr = expArr;
        	super(menuWidth, menuHeight, "표정 설정");
		}
		
		private function onSliderChange(evn:UIEvent) : * {
			var target = evn.currentTarget as Slider;
			var target_idx:uint = 0;
			for (; target_idx < sliderArr.length; target_idx++) {
				if (sliderArr[target_idx] == target)
					break;
			}
			
			if (target_idx >= sliderArr.length)
				return;
				
			(sliderValueLblArr[target_idx] as Label).text = evn.value;
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SetExpression(target_idx, evn.value);
		}
		
		private function loadBtn_Activated(evn:UIEvent) : * {
			var view = new SelectExpressionsView();
			Shared.ShowView(view);
		}
		
		private function saveBtn_Activated(evn:UIEvent) : * {
			var view = new SaveExpressionView();
			Shared.ShowView(view);
		}
		
		private function resetBtn_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.ResetExpression();
			
			for (var ii = 0; ii < this.sliderArr.length; ii++) {
				(this.sliderArr[ii] as Slider).value = 0;
				(this.sliderValueLblArr[ii] as Label).text = "0";
			}
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			this.x = 940;
			this.y = 20;
			
			panel = new Panel(menuWidth - 20, menuHeight - 175);
			panel.x = 10;
			panel.y = 55;
			this.AddComponent(panel);
			
			this.sliderArr = new Array();
			this.sliderValueLblArr = new Array();
			
			var nextPos:Number = 0;
			var sliderNameLbl:Label;
			var nSlider:Slider;
			var nSliderValLbl:Label;
			
			for (var ii:uint = 0; ii < 54; ii++) {
				sliderNameLbl = new Label(30, 28, ii.toString(), 18);
				sliderNameLbl.x = 5;
				sliderNameLbl.y = nextPos + 5;
				panel.AddComponent(sliderNameLbl);
				
				nSlider = new Slider(menuWidth - 75, 24, 0, 100, 1);
				nSlider.x = 10;
				nSlider.y = nextPos + 30;
				nSlider.addEventListener(UIEvent.CHANGE, onSliderChange);
				nSlider.value = this._sliderInitValArr[ii];
				this.sliderArr.push(nSlider);
				panel.AddComponent(nSlider);
				
				nSliderValLbl = new Label(40, 30, this._sliderInitValArr[ii].toString(), 20);
				nSliderValLbl.x = 255;
				nSliderValLbl.y = nextPos + 28;
				this.sliderValueLblArr.push(nSliderValLbl);
				panel.AddComponent(nSliderValLbl);
				
				nextPos += 60;
			}
			
			loadBtn = new Button(menuWidth - 20, 30);
			loadBtn.x = 10;
			loadBtn.y = 570;
			loadBtn.text = "불러오기";
			loadBtn.addEventListener(UIEvent.ACTIVATE, loadBtn_Activated);
			this.AddComponent(loadBtn);
			
			saveBtn = new Button(menuWidth - 20, 30);
			saveBtn.x = 10;
			saveBtn.y = 605;
			saveBtn.text = "저장하기";
			saveBtn.addEventListener(UIEvent.ACTIVATE, saveBtn_Activated);
			this.AddComponent(saveBtn);
			
			resetBtn = new Button(menuWidth - 20, 30);
			resetBtn.x = 10;
			resetBtn.y = 640;
			resetBtn.text = "초기화";
			resetBtn.addEventListener(UIEvent.ACTIVATE, resetBtn_Activated);
			this.AddComponent(resetBtn);
		}
	}
}