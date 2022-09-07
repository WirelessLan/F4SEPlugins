package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	
	public class PoseListView extends View {		
		private var menuWidth:uint = 300;
		private var menuHeight:uint = 620;
		
		private var pluginNameLbl:Label;
		private var poseList:ListBox;
		
		private var pluginName:String;

		public function PoseListView(selectedPlugin:String, selectedPose:String) {
			this.pluginName = selectedPlugin;
        	super(menuWidth, menuHeight, "포즈 선택");
			
			GetPoseList();
			
			if (selectedPose) {
				this.poseList.Highlight(selectedPose);
				this.focusedComponent = this.poseList;
			}
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			pluginNameLbl = new Label(menuWidth, 20, this.pluginName, 16);
			pluginNameLbl.x = 0;
			pluginNameLbl.y = 50;
			this.AddComponent(pluginNameLbl);
			
			poseList = new ListBox(menuWidth - 20, menuHeight - 90);
			poseList.x = 10;
			poseList.y = 80;
			poseList.addEventListener(UIEvent.ACTIVATE, poseList_Activated);
			
			this.AddComponent(poseList);
		}
		
		private function GetPoseList() {
			var items:Array;
			if (Shared.F4SEPlugin)
				items = Shared.F4SEPlugin.GetPoseList(pluginName);
			else
				items = new Array("TestPose1", "TestPose2");
				
			this.poseList.SetItems(items);
		}
		
		private function poseList_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SelectPose(this.pluginName, this.poseList.SelectedItem);
				
			Shared.CloseMenu(0);
		}
	}
}