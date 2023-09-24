package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;

	public class SelectSavesView extends View {
		private var menuWidth:uint = 320;
		private var menuHeight:uint = 640;

		private var nodeList:ListBox;

		public function SelectSavesView() {
			var titleText = "_$PNIOPAMenuLoadSave";
			if (Shared.Localizations.$PNIOPAMenuLoadSave)
				titleText = Shared.Localizations.$PNIOPAMenuLoadSave;

			super(menuWidth, menuHeight, titleText);

			nodeList.SetItems(GetSaves());
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

		private function GetSaves() : Array {
			if (!Shared.F4SEPlugin)
				return new Array();
			return Shared.F4SEPlugin.GetSaves();
		}

		private function OnNodeSelected(evn:UIEvent) : * {
			if (!nodeList.SelectedItem)
				return;

			var view = new SaveOptionView(nodeList.SelectedItem)
			Shared.ShowView(view);
		}
	}
}