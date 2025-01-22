from helpers.utils import set_page

class Pages:
    """
    This class manages what page to display on the screen.
    """
    def __init__(self, start=0, total_pages=3, exclude_pages=None):
        """
        This is for initializing the pages settings
        """
        self.current_page = start
        self.total_pages = total_pages - 1
        self.exclude_pages = exclude_pages if exclude_pages else []
        set_page(self.current_page)

    def next_page(self):
        """
        This is for switching view to the next page
        """
        self.current_page = (self.current_page + 1) % (self.total_pages + 1)
        set_page(self.current_page)

    def refresh_page(self):
        """
        This is for refreshing the page in case of value change by the sensor
        """
        if self.current_page not in self.exclude_pages:
            set_page(self.current_page)
