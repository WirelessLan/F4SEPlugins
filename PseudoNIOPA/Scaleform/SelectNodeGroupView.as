package  {
	import UIComponent.View;
	import UIComponent.Button;
	import UIComponent.UIEvent;
	
	public class SelectNodeGroupView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 255;
		
		private var searchNodeBtn:Button;
		private var upperNodesBtn:Button;
		private var lowerNodesBtn:Button;
		private var fingerNodesBtn:Button;
		private var etcNodesBtn:Button;
		
		private var selectedNode:String;

		public function SelectNodeGroupView(selectedNode:String) {
			this.selectedNode = selectedNode;
			
			var titleText = "_$PNIOPAMenuSelectNode";
			if (Shared.Localizations.$PNIOPAMenuSelectNode)
				titleText = Shared.Localizations.$PNIOPAMenuSelectNode;
        	super(menuWidth, menuHeight, titleText);
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			var nextBtnPos:Number = 75;
			
			searchNodeBtn = new Button(menuWidth - 20, 30);
			searchNodeBtn.x = 10;
			searchNodeBtn.y = nextBtnPos;
			var searchNodeBtnText = "_$PNIOPAMenuSearchNode";
			if (Shared.Localizations.$PNIOPAMenuSearchNode)
				searchNodeBtnText = Shared.Localizations.$PNIOPAMenuSearchNode;
			searchNodeBtn.text = searchNodeBtnText;
			searchNodeBtn.addEventListener(UIEvent.ACTIVATE, searchNodeBtn_Activated);
			this.AddComponent(searchNodeBtn);
			
			nextBtnPos += 35;
			
			upperNodesBtn = new Button(menuWidth - 20, 30);
			upperNodesBtn.x = 10;
			upperNodesBtn.y = nextBtnPos;
			var upperNodesBtnText = "_$PNIOPAMenuUpperNodes";
			if (Shared.Localizations.$PNIOPAMenuUpperNodes)
				upperNodesBtnText = Shared.Localizations.$PNIOPAMenuUpperNodes;
			upperNodesBtn.text = upperNodesBtnText;
			upperNodesBtn.addEventListener(UIEvent.ACTIVATE, OnButtonActivated);
			this.AddComponent(upperNodesBtn);
			
			nextBtnPos += 35;
			
			lowerNodesBtn = new Button(menuWidth - 20, 30);
			lowerNodesBtn.x = 10;
			lowerNodesBtn.y = nextBtnPos;
			var lowerNodesBtnText = "_$PNIOPAMenuLowerNodes";
			if (Shared.Localizations.$PNIOPAMenuLowerNodes)
				lowerNodesBtnText = Shared.Localizations.$PNIOPAMenuLowerNodes;
			lowerNodesBtn.text = lowerNodesBtnText;
			lowerNodesBtn.addEventListener(UIEvent.ACTIVATE, OnButtonActivated);
			this.AddComponent(lowerNodesBtn);
			
			nextBtnPos += 35;
			
			fingerNodesBtn = new Button(menuWidth - 20, 30);
			fingerNodesBtn.x = 10;
			fingerNodesBtn.y = nextBtnPos;
			var fingerNodesBtnText = "_$PNIOPAMenuFingerNodes";
			if (Shared.Localizations.$PNIOPAMenuFingerNodes)
				fingerNodesBtnText = Shared.Localizations.$PNIOPAMenuFingerNodes;
			fingerNodesBtn.text = fingerNodesBtnText;
			fingerNodesBtn.addEventListener(UIEvent.ACTIVATE, OnButtonActivated);
			this.AddComponent(fingerNodesBtn);
			
			nextBtnPos += 35;
			
			etcNodesBtn = new Button(menuWidth - 20, 30);
			etcNodesBtn.x = 10;
			etcNodesBtn.y = nextBtnPos;
			var etcNodesBtnText = "_$PNIOPAMenuETCNodes";
			if (Shared.Localizations.$PNIOPAMenuETCNodes)
				etcNodesBtnText = Shared.Localizations.$PNIOPAMenuETCNodes;
			etcNodesBtn.text = etcNodesBtnText;
			etcNodesBtn.addEventListener(UIEvent.ACTIVATE, OnButtonActivated);
			this.AddComponent(etcNodesBtn);
		}
		
		private function searchNodeBtn_Activated(evn:UIEvent) : * {
			var nodeName:String;
			if (Shared.F4SEPlugin)
				nodeName = Shared.F4SEPlugin.GetNodeNameForSearch();
			else
				nodeName = "Root";
			
			if (!nodeName) {
				var errTitle = "_$PNIOPAMenuError";
				if (Shared.Localizations.$PNIOPAMenuError)
					errTitle = Shared.Localizations.$PNIOPAMenuError;
					
				var errMsg = "_$PNIOPAMenuGetNodeError";
				if (Shared.Localizations.$PNIOPAMenuGetNodeError)
					errMsg = Shared.Localizations.$PNIOPAMenuGetNodeError;
					
				Shared.ShowMessageBox(errTitle, errMsg);
				return;
			}
			var view = new SearchNodeView(nodeName);
			Shared.ShowView(view);
		}
		
		private function OnButtonActivated(evn:UIEvent) : * {
			var btn:Button = evn.currentTarget as Button;
			if (!btn)
				return;
			
			var node:String = btn.text;
			
			var view = new SelectNodeView(node, this.selectedNode);
			Shared.ShowView(view);
		}
	}
}