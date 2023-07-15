package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	
	public class SelectChildNodesView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 600;
		
		private var nodeList:ListBox;

		public function SelectChildNodesView(childNodes:Array) {
			var titleText = "_$PNIOPAMenuSelectChildNodesBtn";
			if (Shared.Localizations.$PNIOPAMenuSelectChildNodesBtn)
				titleText = Shared.Localizations.$PNIOPAMenuSelectChildNodesBtn;
        	super(menuWidth, menuHeight, titleText);
			
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
						
			Shared.CloseView(2);
			var view = new SearchNodeView(nodeList.SelectedItem);
			Shared.ShowView(view);
		}
	}
}