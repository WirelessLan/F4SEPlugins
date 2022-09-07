package UIComponent {
	import flash.events.Event;

	public class UIEvent extends Event {
		public static const ACTIVATE:String = "ui_activate";
		public static const CHANGE:String = "ui_change";
		
		private var _value:*;
		
		public function UIEvent(type:String, value:* = null, bubbles:Boolean = false, cancelable:Boolean = false) {
            super(type, bubbles, cancelable);
			this._value = value;
        }
		
		public function get value() : * {
			return this._value;
		}
	}
}