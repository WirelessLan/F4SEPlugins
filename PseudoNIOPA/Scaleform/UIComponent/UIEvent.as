package UIComponent {
	import flash.events.Event;

	public class UIEvent extends Event {
		public static const ACTIVATE:String = "ui_activate";

		public function UIEvent(type:String, bubbles:Boolean = false, cancelable:Boolean = false) {
			super(type, bubbles, cancelable);
		}
	}
}