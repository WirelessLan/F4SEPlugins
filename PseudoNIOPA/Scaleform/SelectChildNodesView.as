package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	
	public class SelectChildNodesView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 600;
		
		private var nodeList:ListBox;

		public function SelectChildNodesView(childNodes:Array) {
        	super(menuWidth, menuHeight, "자식 노드 조회");
			
			nodeList.SetItems(childNodes);
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			nodeList = new ListBox(menuWidth - 20, menuHeight - 86);
			nodeList.x = 10;
			nodeList.y = 66;
			nodeList.addEventListener(UIEvent.ACTIVATE, OnNodeSelected);
			this.AddComponent(nodeList);
		}
		
		private function OnNodeSelected(evn:UIEvent) : * {
			if (!nodeList.SelectedItem)
				return;
						
			Shared.CloseMenu(2);
			var view = new SearchNodeView(nodeList.SelectedItem);
			Shared.ShowView(view);
		}
	}
}