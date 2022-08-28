package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	
	public class SelectSavesView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 600;
		
		private var nodeList:ListBox;

		public function SelectSavesView() {
        	super(menuWidth, menuHeight, "불러오기");
			
			nodeList.SetItems(GetSaves());
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			nodeList = new ListBox(menuWidth - 20, menuHeight - 86);
			nodeList.x = 10;
			nodeList.y = 66;
			nodeList.addEventListener(UIEvent.ACTIVATE, OnNodeSelected);
			this.AddComponent(nodeList);
		}
		
		private function GetSaves() : Array {
			if (Shared.F4SEPlugin)
				return Shared.F4SEPlugin.GetSaves();
			else
				return new Array();
		}
		
		private function OnNodeSelected(evn:UIEvent) : * {
			if (!nodeList.SelectedItem)
				return;

			var view = new SaveOptionView(nodeList.SelectedItem)
			Shared.ShowView(view);
		}
	}
}