package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	
	public class PluginListView extends View {
		private var menuWidth:uint = 300;
		private var menuHeight:uint = 620;
		
		private var pluginList:ListBox;

		public function PluginListView(selectedPlugin:String) {
        	super(menuWidth, menuHeight, "플러그인 선택");
			
			GetPluginList();
			
			if (selectedPlugin) {
				this.pluginList.Highlight(selectedPlugin);
				this.focusedComponent = this.pluginList;
			}
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			pluginList = new ListBox(menuWidth - 20, menuHeight - 65);
			pluginList.x = 10;
			pluginList.y = 55;
			pluginList.addEventListener(UIEvent.ACTIVATE, pluginList_Activated);
			
			this.AddComponent(pluginList);
		}
		
		private function GetPluginList() {
			var items:Array;
			if (Shared.F4SEPlugin)
				items = Shared.F4SEPlugin.GetPluginList();
			else
				items = new Array("TestPlugin1.esp", "TestPlugin2.esp", "TestPlugin3.esp");
				
			this.pluginList.SetItems(items);
		}
		
		private function pluginList_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SelectPlugin(this.pluginList.SelectedItem);

			this.pluginList.Highlight(this.pluginList.SelectedItem);
			Shared.ShowPoseListView(this.pluginList.SelectedItem, null);
		}
	}
}