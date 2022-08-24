package  {
	import flash.text.TextFormat;
	import flash.text.TextField;
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	
	public class PoseListView extends View {		
		private var menuWidth:uint = 300;
		private var menuHeight:uint = 620;
		
		private var pluginNameText:TextField;
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
			
			pluginNameText = new TextField();
			pluginNameText.selectable = false;
			pluginNameText.embedFonts = true;
			pluginNameText.text = this.pluginName;
			pluginNameText.x = 0;
			pluginNameText.y = 50;
			pluginNameText.width = menuWidth;
			
			var pluginNameText_tf:TextFormat = pluginNameText.getTextFormat();
			pluginNameText_tf.color = 0xFFFFFF;
			pluginNameText_tf.font = Shared.MainFont.fontName;
			pluginNameText_tf.size = 16;
			pluginNameText_tf.kerning = true;
			pluginNameText_tf.align = "center";
			
			pluginNameText.setTextFormat(pluginNameText_tf);
			
			this.AddComponent(pluginNameText);
			
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