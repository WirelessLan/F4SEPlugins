package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;

	public class SelectNodeView extends View {
		private var menuWidth:uint = 320;
		private var menuHeight:uint = 640;

		private var nodeList:ListBox;
		private var group:String;

		public function SelectNodeView(group:String, selectedNode:String) {
			this.group = group;

			var titleText = "_$PNIOPAMenuSelectNode";
			if (Shared.Localizations.$PNIOPAMenuSelectNode)
				titleText = Shared.Localizations.$PNIOPAMenuSelectNode;

			super(menuWidth, menuHeight, titleText);

			nodeList.SetItems(GetNodes(this.group));

			if (selectedNode) {
				this.nodeList.Highlight(selectedNode);
				this.focusedComponent = this.nodeList;
			}
		}

		protected override function InitializeView() : void {
			super.InitializeView();

			this.x = 20;
			this.y = 10;

			nodeList = new ListBox(menuWidth - 20, menuHeight - 20);
			nodeList.x = 10;
			nodeList.y = 55;
			nodeList.addEventListener(UIEvent.ACTIVATE, OnNodeSelected);
			this.AddComponent(nodeList);
		}

		private function GetNodes(group:String) : Array {
			if (Shared.F4SEPlugin)
				return Shared.F4SEPlugin.GetNodeList(group);
			else 
				return new Array('COM', 'Root');
		}

		private function OnNodeSelected(evn:UIEvent) : * {
			if (!nodeList.SelectedItem)
				return;

			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SelectNode(nodeList.SelectedItem);
			Shared.CloseView(0);
		}
	}
}