from utils import set_page

class Pages:
    def __init__(self, start=0, total_pages=5, exclude_pages=None):
        self.current_page = start
        self.total_pages = total_pages
        self.exclude_pages = exclude_pages if exclude_pages else []
        set_page(self.current_page)

    def next_page(self):
        self.current_page = (self.current_page + 1) % (self.total_pages + 1)
        set_page(self.current_page)

    def refresh_page(self):
        if self.current_page not in self.exclude_pages:
            set_page(self.current_page)
