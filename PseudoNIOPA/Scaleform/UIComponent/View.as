package UIComponent {
	import flash.display.Sprite;
	import flash.geom.Rectangle;
	import flash.events.MouseEvent;
	import flash.display.DisplayObject;
	import UIComponent.IComponent;
	import UIComponent.Label;
	import UIComponent.FocusDirection;
	
	public class View extends Sprite {		
		protected var componentList:Array;
		protected var focusedComponent:IComponent;
		
		private var titleLbl:Label;
		
		private var _title:String;
		private var _width:Number;
		private var _height:Number;

		public function View(width:Number, height:Number, title:String) {
			this._width = width;
			this._height = height;
			this._title = title;
			this.componentList = new Array();
			this.focusedComponent = null;
			
			InitializeView();
		}
		
		private function findFirstFocusableComponent() : IComponent {
			for (var com_idx:int = 0; com_idx < this.componentList.length; com_idx++)
				if ((this.componentList[com_idx] as IComponent).IsFocusable())
					return this.componentList[com_idx] as IComponent;
					
			return null;
		}
		
		private function getComponentIndex(component:IComponent) : int {
			if (!component)
				return -1;
				
			var currComponentIndex:int = 0;
			for (; currComponentIndex < this.componentList.length; currComponentIndex++)
				if (this.componentList[currComponentIndex] == component)
					break;
			
			if (currComponentIndex == this.componentList.length)
				return -1;

			return currComponentIndex;
		}
		
		private function findPreviousFocusableComponent(currComponent:IComponent) : IComponent {
			var currComponentIndex:int = getComponentIndex(currComponent);				
			if (currComponentIndex < 0)
				return null;
			
			var prevComponentIndex:int = currComponentIndex - 1;
			for (; prevComponentIndex >= 0; prevComponentIndex--)
				if ((this.componentList[prevComponentIndex] as IComponent).IsFocusable())
					break;

			if (prevComponentIndex < 0)
				return null;
				
			return this.componentList[prevComponentIndex];
		}
		
		private function findNextFocusableComponent(currComponent:IComponent) : IComponent {
			var currComponentIndex:int = getComponentIndex(currComponent);				
			if (currComponentIndex < 0)
				return null;
			
			var nextComponentIndex:int = currComponentIndex + 1;
			for (; nextComponentIndex < this.componentList.length; nextComponentIndex++)
				if ((this.componentList[nextComponentIndex] as IComponent).IsFocusable())
					break;

			if (nextComponentIndex >= this.componentList.length)
				return null;
				
			return this.componentList[nextComponentIndex];
		}
		
		public function ProcessKeyEvent(ctrlName:String) : void {
			if (ctrlName == "Activate") {
				if (this.focusedComponent)
					this.focusedComponent.Activate();
			}
			else if (ctrlName == "Forward") {
				if (this.focusedComponent) {
					if (this.focusedComponent.CanChangeFocus(FocusDirection.Previous)) {
						var prevComponent:IComponent = findPreviousFocusableComponent(this.focusedComponent);
						if (prevComponent) {
							this.focusedComponent.SetFocus(false, FocusDirection.Previous);
							this.focusedComponent = prevComponent;
							this.focusedComponent.SetFocus(true, FocusDirection.Previous);
						}
					}
					else {
						this.focusedComponent.SetInnerFocus(FocusDirection.Previous);
					}
				}
				else {
					var firstFocusableComponent:IComponent = findFirstFocusableComponent();
					if (firstFocusableComponent) {
						this.focusedComponent = firstFocusableComponent;
						this.focusedComponent.SetFocus(true, FocusDirection.Previous);
					}
				}
			}
			else if (ctrlName == "Back") {
				if (this.focusedComponent) {
					if (this.focusedComponent.CanChangeFocus(FocusDirection.Next)) {
						var nextComponent:IComponent = findNextFocusableComponent(this.focusedComponent);
						if (nextComponent) {
							this.focusedComponent.SetFocus(false, FocusDirection.Next);
							this.focusedComponent = nextComponent;
							this.focusedComponent.SetFocus(true, FocusDirection.Next);
						}
					}
					else {
						this.focusedComponent.SetInnerFocus(FocusDirection.Next);
					}
				}
				else {
					firstFocusableComponent = findFirstFocusableComponent();
					if (firstFocusableComponent) {
						this.focusedComponent = firstFocusableComponent;
						this.focusedComponent.SetFocus(true, FocusDirection.Next);
					}
				}
			}
		}
		
		public function AddComponent(component:DisplayObject) : void {
			if (component is IComponent) {
				this.componentList.push(component);
				component.addEventListener(MouseEvent.ROLL_OVER, onMouseOver);
			}
			
			this.addChild(component);
		}
		
		protected function InitializeView() : void {
			this.x = (Shared.UIWidth - this._width) / 2;
			this.y = (Shared.UIHeight - this._height) / 2;
			
			this.graphics.beginFill(Shared.Color_Background, 0.8);
			this.graphics.drawRect(0, 0, this._width, this._height);
			this.graphics.endFill();
			
			this.graphics.lineStyle(2, Shared.Color_Primary);
			this.graphics.drawRect(0, 0, this._width, this._height);
			
			var unlineWidth:uint = this._width - 20;
			this.graphics.lineStyle(1, Shared.Color_Primary);
			this.graphics.moveTo((this._width - unlineWidth) / 2, 45);
			this.graphics.lineTo((this._width - unlineWidth) / 2 + unlineWidth, 45);
			
			titleLbl = new Label(this._width, 38, this._title, 30);
			titleLbl.x = 0;
			titleLbl.y = 5;
			this.AddComponent(titleLbl);
		}
		
		private function onMouseOver(evn:MouseEvent) : * {
			evn.stopPropagation();
			
			if (!evn.currentTarget is IComponent)
				return;
				
			var tmpComp:IComponent = evn.currentTarget as IComponent;
			if (tmpComp == this.focusedComponent || !tmpComp.IsFocusable())
				return;
				
			if (this.focusedComponent)
				this.focusedComponent.SetFocus(false, FocusDirection.Next);
			this.focusedComponent = evn.currentTarget as IComponent;
			this.focusedComponent.SetFocus(true, FocusDirection.Next);
		}
	}
}