package UIComponent {
	public interface IComponent {
		function Activate() : void;
		function CanChangeFocus(dir:int) : Boolean;
		function IsFocusable() : Boolean;
		function SetFocus(onOff:Boolean) : void;
		function ProcessKeyEvent(keyCode:uint) : void;
	}
}