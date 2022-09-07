package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	
	public class SelectExpressionsView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 600;
		
		private var expList:ListBox;

		public function SelectExpressionsView() {
        	super(menuWidth, menuHeight, "표정 불러오기");
			
			expList.SetItems(GetExpressions());
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			expList = new ListBox(menuWidth - 20, menuHeight - 77);
			expList.x = 10;
			expList.y = 66;
			expList.addEventListener(UIEvent.ACTIVATE, OnNodeSelected);
			this.AddComponent(expList);
		}
		
		private function GetExpressions() : Array {
			if (Shared.F4SEPlugin)
				return Shared.F4SEPlugin.GetExpressions();
			else
				return new Array();
		}
		
		private function OnNodeSelected(evn:UIEvent) : * {
			if (!expList.SelectedItem)
				return;
				
			var view = new LoadExpressionView(expList.SelectedItem);
			Shared.ShowView(view);
		}
	}
}