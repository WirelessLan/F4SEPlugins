package UIComponent {
	import flash.display.Sprite;
	import flash.display.DisplayObject;
	import flash.geom.Rectangle;
	import flash.ui.Keyboard;
	import flash.events.MouseEvent;
	
	public class BaseContainer extends Sprite {
		protected var _width:Number;
		protected var _height:Number;
		
		protected var _componentArr:Array;
		
		protected var componentContainer:Sprite;
		protected var focusedComponent:IComponent;

		public function BaseContainer(width:Number, height:Number) {
			super();
			
			this._width = width;
			this._height = height;
			
			this._componentArr = new Array();
			this.focusedComponent = null;
			
			initializeComponent();
		}
		
		public function AddComponent(component:DisplayObject) : void {
			if (component is IComponent) {
				this._componentArr.push(component);
				component.addEventListener(MouseEvent.ROLL_OVER, onMouseOver);
			}
			
			this.componentContainer.addChild(component);
		}
		
		public function ProcessKeyEvent(keyCode:uint) : void {
			if (keyCode == Keyboard.ENTER) {
				if (this.focusedComponent)
					this.focusedComponent.Activate();
			}
			else if (keyCode == Keyboard.UP) {
				if (this.focusedComponent) {
					if (this.focusedComponent.CanChangeFocus(FocusDirection.Previous)) {
						var prevComponent:IComponent = FindPreviousFocusableComponent(this.focusedComponent);
						if (prevComponent) {
							this.focusedComponent.SetFocus(false);
							this.focusedComponent = prevComponent;
							this.focusedComponent.SetFocus(true);
							this.focusedComponent.ProcessKeyEvent(keyCode);
						}
					}
					else {
						this.focusedComponent.ProcessKeyEvent(keyCode);
					}
				}
				else {
					var lastFocusableComponent:IComponent = FindLastFocusableComponent();
					if (lastFocusableComponent) {
						this.focusedComponent = lastFocusableComponent;
						this.focusedComponent.SetFocus(true);
						this.focusedComponent.ProcessKeyEvent(keyCode);
					}
				}
			}
			else if (keyCode == Keyboard.DOWN) {
				if (this.focusedComponent) {
					if (this.focusedComponent.CanChangeFocus(FocusDirection.Next)) {
						var nextComponent:IComponent = FindNextFocusableComponent(this.focusedComponent);
						if (nextComponent) {
							this.focusedComponent.SetFocus(false);
							this.focusedComponent = nextComponent;
							this.focusedComponent.SetFocus(true);
							this.focusedComponent.ProcessKeyEvent(keyCode);
						}
					}
					else {
						this.focusedComponent.ProcessKeyEvent(keyCode);
					}
				}
				else {
					var firstFocusableComponent:IComponent = FindFirstFocusableComponent();
					if (firstFocusableComponent) {
						this.focusedComponent = firstFocusableComponent;
						this.focusedComponent.SetFocus(true);
						this.focusedComponent.ProcessKeyEvent(keyCode);
					}
				}
			}
			else {
				if (this.focusedComponent)
					this.focusedComponent.ProcessKeyEvent(keyCode);
			}
		}
		
		protected function FindFirstFocusableComponent() : IComponent {
			for (var com_idx:int = 0; com_idx < this._componentArr.length; com_idx++)
				if ((this._componentArr[com_idx] as IComponent).IsFocusable())
					return this._componentArr[com_idx] as IComponent;
					
			return null;
		}
		
		protected function FindLastFocusableComponent() : IComponent {
			for (var com_idx:int = this._componentArr.length - 1; com_idx >= 0; com_idx--)
				if ((this._componentArr[com_idx] as IComponent).IsFocusable())
					return this._componentArr[com_idx] as IComponent;
					
			return null;
		}
		
		protected function FindPreviousFocusableComponent(currComponent:IComponent) : IComponent {
			var currComponentIndex:int = GetComponentIndex(currComponent);				
			if (currComponentIndex < 0)
				return null;
			
			var prevComponentIndex:int = currComponentIndex - 1;
			for (; prevComponentIndex >= 0; prevComponentIndex--)
				if ((this._componentArr[prevComponentIndex] as IComponent).IsFocusable())
					break;

			if (prevComponentIndex < 0)
				return null;
				
			return this._componentArr[prevComponentIndex];
		}
		
		protected function FindNextFocusableComponent(currComponent:IComponent) : IComponent {
			var currComponentIndex:int = GetComponentIndex(currComponent);				
			if (currComponentIndex < 0)
				return null;
			
			var nextComponentIndex:int = currComponentIndex + 1;
			for (; nextComponentIndex < this._componentArr.length; nextComponentIndex++)
				if ((this._componentArr[nextComponentIndex] as IComponent).IsFocusable())
					break;

			if (nextComponentIndex >= this._componentArr.length)
				return null;
				
			return this._componentArr[nextComponentIndex];
		}
		
		protected function GetComponentIndex(component:IComponent) : int {
			if (!component)
				return -1;
				
			var currComponentIndex:int = 0;
			for (; currComponentIndex < this._componentArr.length; currComponentIndex++)
				if (this._componentArr[currComponentIndex] == component)
					break;
			
			if (currComponentIndex == this._componentArr.length)
				return -1;

			return currComponentIndex;
		}
		
		private function initializeComponent() {
			this.componentContainer = new Sprite();
			this.componentContainer.graphics.beginFill(0xFFFFFF, 0);
			this.componentContainer.graphics.drawRect(0, 0, this._width, this._height);
			this.componentContainer.graphics.endFill();
			this.addChild(this.componentContainer);
		}
		
		private function onMouseOver(evn:MouseEvent) : * {
			evn.stopPropagation();
			
			if (!evn.currentTarget is IComponent)
				return;
				
			var tmpComp:IComponent = evn.currentTarget as IComponent;
			if (tmpComp == this.focusedComponent || !tmpComp.IsFocusable())
				return;
				
			if (this.focusedComponent)
				this.focusedComponent.SetFocus(false);
			this.focusedComponent = evn.currentTarget as IComponent;
			this.focusedComponent.SetFocus(true);
		}
	}
}